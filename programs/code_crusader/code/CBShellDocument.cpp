/******************************************************************************
 CBShellDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 2009 by John Lindal.

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

// string ID's

static const JCharacter* kKillLabelID      = "KillLabel::CBExecOutputDocument";
static const JCharacter* kStopButtonHintID = "StopButtonHint::CBExecOutputDocument";

/******************************************************************************
 Create (static)

 ******************************************************************************/

JBoolean
CBShellDocument::Create
	(
	CBShellDocument** doc
	)
{
	const JString shell     = JGetUserShell();
	const JCharacter* cmd[] = { shell, "-i", NULL };

	JProcess* p;
	int inFD, outFD;
	const JError err = JProcess::Create(&p, cmd, sizeof(cmd), kJCreatePipe, &outFD,
										kJCreatePipe, &inFD, kJAttachToFromFD);
	if (err.OK())
		{
		*doc = jnew CBShellDocument(p, inFD, outFD);
		assert( *doc != NULL );

		(**doc).Activate();
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		*doc = NULL;
		return kJFalse;
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
	CBTextDocument(kCBShellOutputFT, "CBShellHelp", kJFalse, ConstructShellEditor)
{
	itsShellEditor = (CBShellEditor*) GetTextEditor();
	itsProcess     = NULL;
	itsDataLink    = NULL;
	itsCmdStream   = NULL;

	// buttons in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	JXMenuBar* menuBar = GetMenuBar();
	const JSize h      = menuBar->GetFrameHeight();

	const JCoordinate x = 2 * kMenuButtonWidth;

	itsKillButton =
		jnew JXTextButton(JGetString(kKillLabelID), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != NULL );
	ListenTo(itsKillButton);

	itsKillButton->SetShortcuts("^C#.");
	itsKillButton->SetHint(JGetString(kStopButtonHintID));

	menuBar->AdjustSize(-kMenuButtonWidth, 0);

	SetConnection(p, inFD, outFD);

	window->SetWMClass(CBGetWMClassInstance(), CBGetShellWindowClass());
}

// static private

CBTextEditor*
CBShellDocument::ConstructShellEditor
	(
	CBTextDocument*		document,
	const JCharacter*	fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	CBShellEditor* te =
		jnew CBShellEditor(document, fileName, menuBar, lineInput, colInput,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != NULL );

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
	assert( !ProcessRunning() && itsDataLink == NULL );

	itsProcess = p;
	ListenTo(itsProcess);

	itsDataLink = new DataLink(inFD);
	assert( itsDataLink != NULL );
	ListenTo(itsDataLink);

	itsCmdStream = jnew JOutPipeStream(outFD, kJTrue);
	assert( itsCmdStream != NULL );

	UpdateButtons();
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

void
CBShellDocument::DeleteLinks()
{
	delete itsDataLink;
	itsDataLink = NULL;
}

/******************************************************************************
 SendCommand

 ******************************************************************************/

void
CBShellDocument::SendCommand
	(
	const JCharacter* cmd
	)
{
	if (itsCmdStream != NULL)
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
	const JAsynchDataReceiverT::DataReady* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != NULL );

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
	itsCmdStream = NULL;
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
