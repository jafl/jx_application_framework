/******************************************************************************
 CBShellDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#include "CBShellDocument.h"
#include "CBShellEditor.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JOutPipeStream.h>
#include <jSysUtil.h>
#include <jAssert.h>

const JSize kMenuButtonWidth = 60;

/******************************************************************************
 Create (static)

 ******************************************************************************/

bool
CBShellDocument::Create
	(
	CBShellDocument** doc
	)
{
	const JString shell    = JGetUserShell();
	const JUtf8Byte* cmd[] = { shell.GetBytes(), "-i", nullptr };

	JProcess* p;
	int inFD, outFD;
	const JError err = JProcess::Create(&p, cmd, sizeof(cmd), kJCreatePipe, &outFD,
										kJCreatePipe, &inFD, kJAttachToFromFD);
	if (err.OK())
		{
		*doc = jnew CBShellDocument(p, inFD, outFD);
		assert( *doc != nullptr );

		(**doc).Activate();
		return true;
		}
	else
		{
		err.ReportIfError();
		*doc = nullptr;
		return false;
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBShellDocument::CBShellDocument
	(
	JProcess*	p,
	const int	inFD,
	const int	outFD
	)
	:
	CBTextDocument(kCBShellOutputFT, "CBShellHelp", false, ConstructShellEditor)
{
	itsShellEditor = (CBShellEditor*) GetTextEditor();
	itsProcess     = nullptr;
	itsDataLink    = nullptr;
	itsCmdStream   = nullptr;

	// buttons in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	JXMenuBar* menuBar = GetMenuBar();
	const JSize h      = menuBar->GetFrameHeight();

	itsKillButton =
		jnew JXTextButton(JGetString("KillLabel::CBExecOutputDocument"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != nullptr );
	ListenTo(itsKillButton);

	itsKillButton->SetShortcuts(JGetString("KillButtonShortcuts::CBShellDocument"));
	itsKillButton->SetHint(JGetString("StopButtonHint::CBExecOutputDocument"));

	menuBar->AdjustSize(-kMenuButtonWidth, 0);

	SetConnection(p, inFD, outFD);

	window->SetWMClass(CBGetWMClassInstance(), CBGetShellWindowClass());
}

// static private

CBTextEditor*
CBShellDocument::ConstructShellEditor
	(
	CBTextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	auto* te =
		jnew CBShellEditor(document, fileName, menuBar, lineInput, colInput,
						   scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBShellDocument::~CBShellDocument()
{
	DeleteLinks();
	CloseOutFD();

	jdelete itsProcess;
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

void
CBShellDocument::SetConnection
	(
	JProcess*	p,
	const int	inFD,
	const int	outFD
	)
{
	assert( !ProcessRunning() && itsDataLink == nullptr );

	itsProcess = p;
	ListenTo(itsProcess);

	itsDataLink = new DataLink(inFD);
	assert( itsDataLink != nullptr );
	ListenTo(itsDataLink);

	itsCmdStream = jnew JOutPipeStream(outFD, true);
	assert( itsCmdStream != nullptr );

	UpdateButtons();
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

void
CBShellDocument::DeleteLinks()
{
	delete itsDataLink;
	itsDataLink = nullptr;
}

/******************************************************************************
 SendCommand

 ******************************************************************************/

void
CBShellDocument::SendCommand
	(
	const JString& cmd
	)
{
	if (itsCmdStream != nullptr)
		{
		*itsCmdStream << cmd;
		itsCmdStream->flush();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBShellDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDataLink && message.Is(JAsynchDataReceiverT::kDataReady))
		{
		ReceiveData(message);
		}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		DeleteLinks();
		CloseOutFD();
		UpdateButtons();
		}

	else if (sender == itsKillButton && message.Is(JXButton::kPushed))
		{
		KillProcess();
		}

	else
		{
		CBTextDocument::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveData (private)

 ******************************************************************************/

void
CBShellDocument::ReceiveData
	(
	const Message& message
	)
{
	const auto* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != nullptr );

	itsShellEditor->InsertText(info->GetData());
}

/******************************************************************************
 KillProcess (protected)

 ******************************************************************************/

void
CBShellDocument::KillProcess()
{
	if (ProcessRunning())
		{
		itsProcess->Kill();
		}
}

/******************************************************************************
 CloseOutFD (private)

	Caller must call UpdateButtons() afterwards.

 ******************************************************************************/

void
CBShellDocument::CloseOutFD()
{
	jdelete itsCmdStream;
	itsCmdStream = nullptr;
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CBShellDocument::UpdateButtons()
{
	if (ProcessRunning())
		{
		itsKillButton->Activate();
		}
	else
		{
		itsKillButton->Deactivate();
		}
}
