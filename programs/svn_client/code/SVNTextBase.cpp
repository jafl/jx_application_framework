/******************************************************************************
 SVNTextBase.cpp

	BASE CLASS = JXStaticText, SVNTabBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNTextBase.h"
#include "svnMenus.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <JFontManager.h>
#include <JProcess.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Derived class must call RefreshContent()

 ******************************************************************************/

SVNTextBase::SVNTextBase
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStaticText(JString::empty, false, true, true, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	SVNTabBase(director),
	itsProcess(nullptr),
	itsLink(nullptr)
{
	GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());

	FitToEnclosure();

	ShareEditMenu(editMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNTextBase::~SVNTextBase()
{
	if (itsProcess != nullptr)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	jdelete itsProcess;
	itsProcess = nullptr;	// avoid crash in ReceiveGoingAway()

	DeleteLink();
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNTextBase::RefreshContent()
{
	if (itsProcess != nullptr)
		{
		JProcess* p = itsProcess;
		itsProcess  = nullptr;

		p->Kill();
		jdelete p;

		DeleteLink();
		}

	GetText()->SetText(JString::empty);

	JProcess* p;
	int outFD;
	const JError err = StartProcess(&p, &outFD);
	if (err.OK())
		{
		SetConnection(p, outFD);
		}
	else
		{
		err.ReportIfError();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNTextBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(JAsynchDataReceiverT::kDataReady))
		{
		ReceiveData(message);
		}
	else
		{
		JXStaticText::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SVNTextBase::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsProcess)
		{
		itsProcess = nullptr;
		DeleteLink();
		}
	else
		{
		JXStaticText::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ReceiveData (private)

 ******************************************************************************/

void
SVNTextBase::ReceiveData
	(
	const Message& message
	)
{
	const auto* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != nullptr );

	const DisplayState state = SaveDisplayState();

	SetCaretLocation(GetText()->GetText().GetCharacterCount()+1);
	Paste(info->GetData());
	GetText()->ClearUndo();

	RestoreDisplayState(state);
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
SVNTextBase::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsProcess != nullptr)
		{
		DisplayCursor(kJXBusyCursor);
		}
	else
		{
		JXWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 UpdateActionsMenu (virtual)

 ******************************************************************************/

void
SVNTextBase::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
	menu->EnableItem(kRefreshCmd);
}

/******************************************************************************
 UpdateInfoMenu (virtual)

 ******************************************************************************/

void
SVNTextBase::UpdateInfoMenu
	(
	JXTextMenu* menu
	)
{
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

void
SVNTextBase::SetConnection
	(
	JProcess*	p,
	const int	fd
	)
{
	itsProcess = p;
	ListenTo(itsProcess);
	p->ShouldDeleteWhenFinished();

	itsLink = new DataLink(fd);
	assert( itsLink != nullptr );
	ListenTo(itsLink);
}

/******************************************************************************
 DeleteLink (private)

 ******************************************************************************/

void
SVNTextBase::DeleteLink()
{
	delete itsLink;
	itsLink = nullptr;
}
