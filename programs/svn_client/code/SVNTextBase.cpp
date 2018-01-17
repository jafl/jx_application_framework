/******************************************************************************
 SVNTextBase.cc

	BASE CLASS = JXStaticText, SVNTabBase

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "SVNTextBase.h"
#include "svnMenus.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
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
	JXStaticText("", kJFalse, kJTrue, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	SVNTabBase(director),
	itsProcess(NULL),
	itsLink(NULL)
{
	SetDefaultFont(GetFontManager()->GetDefaultMonospaceFont());

	FitToEnclosure();

	ShareEditMenu(editMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNTextBase::~SVNTextBase()
{
	if (itsProcess != NULL)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	jdelete itsProcess;
	itsProcess = NULL;	// avoid crash in ReceiveGoingAway()

	DeleteLink();
}

/******************************************************************************
 RefreshContent (virtual)

 ******************************************************************************/

void
SVNTextBase::RefreshContent()
{
	if (itsProcess != NULL)
		{
		JProcess* p = itsProcess;
		itsProcess  = NULL;

		p->Kill();
		jdelete p;

		DeleteLink();
		}

	SetText("");

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
		itsProcess = NULL;
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
	const JAsynchDataReceiverT::DataReady* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != NULL );

	const DisplayState state = SaveDisplayState();

	SetCaretLocation(GetTextLength()+1);
	Paste(info->GetData());
	ClearUndo();

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
	if (itsProcess != NULL)
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
	assert( itsLink != NULL );
	ListenTo(itsLink);
}

/******************************************************************************
 DeleteLink (private)

 ******************************************************************************/

void
SVNTextBase::DeleteLink()
{
	delete itsLink;
	itsLink = NULL;
}
