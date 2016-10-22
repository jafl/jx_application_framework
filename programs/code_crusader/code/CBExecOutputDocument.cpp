/******************************************************************************
 CBExecOutputDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBExecOutputDocument.h"
#include "CBTextEditor.h"
#include "CBCmdLineInput.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JOutPipeStream.h>
#include <jDirUtil.h>
#include <jSignal.h>
#include <jTime.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JSize kMenuButtonWidth = 60;
const JSize kEOFButtonWidth  = 40;

// JBroadcaster message types

const JCharacter* CBExecOutputDocument::kFinished = "Finished::CBExecOutputDocument";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecOutputDocument::CBExecOutputDocument
	(
	const CBTextFileType	fileType,
	const JCharacter*		helpSectionName,
	const JBoolean			focusToCmd,
	const JBoolean			allowStop
	)
	:
	CBTextDocument(fileType, helpSectionName, kJFalse),
	itsFocusToCmdFlag(focusToCmd)
{
	itsProcess            = NULL;
	itsRecordLink         = NULL;
	itsDataLink           = NULL;
	itsCmdStream          = NULL;
	itsReceivedDataFlag   = kJFalse;
	itsProcessPausedFlag  = kJFalse;
	itsClearWhenStartFlag = kJTrue;
	itsUseCount           = 0;
	itsDontCloseMsg       = JGetString("WaitCloseMsg::CBExecOutputDocument");

	// buttons in upper right

	JXWindow* window = GetWindow();
	const JRect rect = window->GetBounds();

	JXMenuBar* menuBar = GetMenuBar();
	const JSize h      = menuBar->GetFrameHeight();

	const JCoordinate x = (allowStop ? 3 : 2) * kMenuButtonWidth;

	itsPauseButton =
		jnew JXTextButton(JGetString("PauseLabel::CBExecOutputDocument"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - x,0, kMenuButtonWidth,h);
	assert( itsPauseButton != NULL );
	ListenTo(itsPauseButton);
	itsPauseButton->SetShortcuts("^Z");
	itsPauseButton->SetHint(JGetString("PauseButtonHint::CBExecOutputDocument"));

	if (allowStop)
		{
		itsStopButton =
			jnew JXTextButton(JGetString("StopLabel::CBExecOutputDocument"), window,
							 JXWidget::kFixedRight, JXWidget::kFixedTop,
							 rect.right - 2*kMenuButtonWidth,0, kMenuButtonWidth,h);
		assert( itsStopButton != NULL );
		ListenTo(itsStopButton);
		itsStopButton->SetShortcuts("^C#.");
		itsStopButton->SetHint(JGetString("StopButtonHint::CBExecOutputDocument"));
		}
	else
		{
		itsStopButton = NULL;
		}

	itsKillButton =
		jnew JXTextButton(JGetString("KillLabel::CBExecOutputDocument"), window,
						 JXWidget::kFixedRight, JXWidget::kFixedTop,
						 rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != NULL );
	ListenTo(itsKillButton);

	if (!allowStop)
		{
		itsKillButton->SetShortcuts("^C#.");
		itsKillButton->SetHint(JGetString("StopButtonHint::CBExecOutputDocument"));
		}

	menuBar->AdjustSize(-x, 0);

	// command line input

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect fileRect = GetFileDisplayInfo(&hSizing, &vSizing);

	itsCmdPrompt =
		jnew JXStaticText(JGetString("CmdPrompt::CBExecOutputDocument"), window,
						 JXWidget::kFixedLeft, vSizing,
						 fileRect.left, fileRect.top+3, 0, fileRect.height()-3);
	assert( itsCmdPrompt != NULL );
	itsCmdPrompt->Hide();

	const JCoordinate promptWidth = itsCmdPrompt->GetFrameWidth();

	JXToolBar* toolBar = GetToolBar();
	JCoordinate minWidth, minHeight;
	toolBar->GetWindowMinSize(&minWidth, &minHeight);
	minWidth += promptWidth;
	toolBar->SetWindowMinSize(minWidth, minHeight);

	itsCmdInput =
		jnew CBCmdLineInput(this, window, hSizing, vSizing,
						   fileRect.left + promptWidth, fileRect.top,
						   fileRect.width() - promptWidth - kEOFButtonWidth,
						   fileRect.height());
	assert( itsCmdInput != NULL );
	itsCmdInput->ShareEditMenu(GetTextEditor());
	itsCmdInput->Hide();

	itsEOFButton =
		jnew JXTextButton(JGetString("EOFButtonTitle::CBExecOutputDocument"), window,
						 JXWidget::kFixedRight, vSizing,
						 fileRect.right - kEOFButtonWidth, fileRect.top,
						 kEOFButtonWidth, fileRect.height());
	assert( itsEOFButton != NULL );
	itsEOFButton->SetShortcuts("^D");
	itsEOFButton->Hide();
	ListenTo(itsEOFButton);

	UpdateButtons();
	GetTextEditor()->SetWritable(kJFalse);
	(JXGetDocumentManager())->DocumentMustStayOpen(this, kJTrue);

	window->SetWMClass(CBGetWMClassInstance(), CBGetExecOutputWindowClass());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExecOutputDocument::~CBExecOutputDocument()
{
	delete itsRecordLink;
	itsRecordLink = NULL;

	delete itsDataLink;
	itsDataLink = NULL;

	CloseOutFD();

	jdelete itsProcess;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBExecOutputDocument::Activate()
{
	CBTextDocument::Activate();

	if (IsActive() && itsFocusToCmdFlag)
		{
		itsCmdInput->Focus();
		}
}

/******************************************************************************
 OKToClose (virtual protected)

 ******************************************************************************/

JBoolean
CBExecOutputDocument::OKToClose()
{
	if (itsUseCount > 0 || itsRecordLink != NULL || itsDataLink != NULL)
		{
		Activate();
		(JGetUserNotification())->ReportError(itsDontCloseMsg);
		return kJFalse;
		}
	else
		{
		return CBTextDocument::OKToClose();
		}
}

/******************************************************************************
 Use count

	Not inline because we use assert().

 ******************************************************************************/

void
CBExecOutputDocument::IncrementUseCount()
{
	itsUseCount++;
}

void
CBExecOutputDocument::DecrementUseCount()
{
	assert( itsUseCount > 0 );
	itsUseCount--;

	if (itsUseCount == 0)
		{
		itsClearWhenStartFlag = !ProcessRunning();
		}
}

/******************************************************************************
 SetConnection (virtual)

	execCmd can be NULL.  If it is, the header (timestamp + path + cmd)
	is not printed.

 ******************************************************************************/

void
CBExecOutputDocument::SetConnection
	(
	JProcess*			p,
	const int			inFD,
	const int			outFD,
	const JCharacter*	windowTitle,
	const JCharacter*	dontCloseMsg,
	const JCharacter*	execDir,
	const JCharacter*	execCmd,
	const JBoolean		showPID
	)
{
	assert( !ProcessRunning() && itsRecordLink == NULL && itsDataLink == NULL );

	itsProcess = p;
	ListenTo(itsProcess);

	if (NeedsFormattedData())
		{
		itsRecordLink = new RecordLink(inFD);
		assert( itsRecordLink != NULL );
		ListenTo(itsRecordLink);
		}
	else
		{
		itsDataLink = new DataLink(inFD);
		assert( itsDataLink != NULL );
		ListenTo(itsDataLink);
		}

	if (outFD != ACE_INVALID_HANDLE)
		{
		itsCmdStream = jnew JOutPipeStream(outFD, kJTrue);
		assert( itsCmdStream != NULL );
		}

	CBTextEditor* te = GetTextEditor();
	if (itsClearWhenStartFlag)
		{
		te->SetText("");
		}
	else if (!te->IsEmpty())
		{
		const JString& text = te->GetText();
		JIndex i            = text.GetLength()+1;
		while (i > 1 && text.GetCharacter(i-1) == '\n')
			{
			i--;
			}
		if (text.IndexValid(i))
			{
			te->SetSelection(i, text.GetLength());
			}

		te->Paste("\n\n----------\n\n");
		te->ClearUndo();
		}

	if (execCmd != NULL)
		{
		const JString timeStamp = JGetTimeStamp();

		te->Paste(timeStamp);
		te->Paste("\n");
		te->Paste(execDir);
		te->Paste("\n");
		te->Paste(execCmd);

		if (showPID)
			{
			te->Paste("\n");
			te->Paste(JGetString("ProcessID::CBExecOutputDocument"));
			te->Paste(JString(p->GetPID(), JString::kBase10));
			}

		te->Paste("\n\n");
		te->ClearUndo();
		}

	itsPath              = execDir;
	itsReceivedDataFlag  = kJFalse;
	itsProcessPausedFlag = kJFalse;
	itsDontCloseMsg      = dontCloseMsg;
	FileChanged(windowTitle, kJFalse);

	UpdateButtons();
	te->SetWritable(kJFalse);
	itsCmdInput->SetText("");
}

/******************************************************************************
 SendText

 ******************************************************************************/

void
CBExecOutputDocument::SendText
	(
	const JCharacter* text
	)
{
	if (ProcessRunning() && itsCmdStream != NULL)
		{
		*itsCmdStream << text;
		itsCmdStream->flush();

		CBTextEditor* te = GetTextEditor();
		te->SetCaretLocation(te->GetTextLength()+1);

		if (text[0] != '\0' && text[0] != '\n' &&
			itsRecordLink != NULL &&
			itsRecordLink->PeekPartialMessage(&itsLastPrompt))
			{
			te->Paste(itsLastPrompt);
			}

		te->Paste(text);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBExecOutputDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRecordLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveRecord();
		}
	else if (sender == itsDataLink && message.Is(JAsynchDataReceiverT::kDataReady))
		{
		ReceiveData(message);
		}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != NULL );
		const JBoolean stayOpen = ProcessFinished(*info);

		// let somebody else start a new process

		itsClearWhenStartFlag = kJFalse;	// in case they call SetConnection() in ReceiveWithFeedback()
		Finished msg(info->Successful(), JI2B(info->GetReason() != kJChildFinished));
		BroadcastWithFeedback(&msg);
		itsClearWhenStartFlag = JI2B(itsUseCount == 0 && !msg.SomebodyIsWaiting());

		if (itsUseCount == 0 && !stayOpen && !ProcessRunning())
			{
			Close();
			}
		}

	else if (sender == itsPauseButton && message.Is(JXButton::kPushed))
		{
		ToggleProcessRunning();
		}
	else if (sender == itsStopButton && message.Is(JXButton::kPushed))
		{
		StopProcess();
		}
	else if (sender == itsKillButton && message.Is(JXButton::kPushed))
		{
		KillProcess();
		}

	else if (sender == itsEOFButton && message.Is(JXButton::kPushed))
		{
		if (!(itsCmdInput->GetText()).IsEmpty())
			{
			itsCmdInput->HandleKeyPress(kJReturnKey, JXKeyModifiers(GetDisplay()));
			}
		CloseOutFD();
		UpdateButtons();
		}

	else
		{
		CBTextDocument::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveRecord (private)

 ******************************************************************************/

void
CBExecOutputDocument::ReceiveRecord()
{
	assert( itsRecordLink != NULL );

	JString text;
	const JBoolean ok = itsRecordLink->GetNextMessage(&text);
	assert( ok );

	// remove text that has already been printed

	if (!itsLastPrompt.IsEmpty() && text.BeginsWith(itsLastPrompt))
		{
		text.RemoveSubstring(1, itsLastPrompt.GetLength());
		}
	itsLastPrompt.Clear();

	const JXTEBase::DisplayState state = GetTextEditor()->SaveDisplayState();

	AppendText(text);
	GetTextEditor()->ClearUndo();

	if (!itsReceivedDataFlag)
		{
		itsReceivedDataFlag = kJTrue;
		if (!IsActive())
			{
			Activate();
			}
		}

	GetTextEditor()->RestoreDisplayState(state);
}

/******************************************************************************
 AppendText (virtual protected)

	Append the text to our text editor.  Derived classes can override
	to filter or otherwise process the text.

 ******************************************************************************/

void
CBExecOutputDocument::AppendText
	(
	const JString& text
	)
{
	CBTextEditor* te = GetTextEditor();
	te->SetCaretLocation(te->GetTextLength()+1);
	te->PasteUNIXTerminalOutput(text);
	te->Paste("\n");
}

/******************************************************************************
 ReceiveData (private)

 ******************************************************************************/

void
CBExecOutputDocument::ReceiveData
	(
	const Message& message
	)
{
	const JAsynchDataReceiverT::DataReady* info =
		dynamic_cast<const JAsynchDataReceiverT::DataReady*>(&message);
	assert( info != NULL );

	CBTextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	te->SetCaretLocation(te->GetTextLength()+1);
	te->PasteUNIXTerminalOutput(info->GetData());
	te->ClearUndo();

	if (!itsReceivedDataFlag)
		{
		itsReceivedDataFlag = kJTrue;
		if (!IsActive())
			{
			Activate();
			}
		}

	te->RestoreDisplayState(state);
}

/******************************************************************************
 ToggleProcessRunning (protected)

 ******************************************************************************/

void
CBExecOutputDocument::ToggleProcessRunning()
{
	if (!ProcessRunning())
		{
		return;
		}

	if (!itsProcessPausedFlag)
		{
		if (itsProcess->SendSignalToGroup(SIGSTOP) == kJNoError)
			{
			itsProcessPausedFlag = kJTrue;
			UpdateButtons();
			}
		}
	else
		{
		if (itsProcess->SendSignalToGroup(SIGCONT) == kJNoError)
			{
			itsProcessPausedFlag = kJFalse;
			UpdateButtons();
			}
		}
}

/******************************************************************************
 StopProcess (protected)

 ******************************************************************************/

void
CBExecOutputDocument::StopProcess()
{
	if (ProcessRunning())
		{
		itsProcess->Quit();
		}
}

/******************************************************************************
 KillProcess (protected)

 ******************************************************************************/

void
CBExecOutputDocument::KillProcess()
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
CBExecOutputDocument::CloseOutFD()
{
	if (itsCmdStream != NULL)
		{
		jdelete itsCmdStream;
		itsCmdStream = NULL;
		}
}

/******************************************************************************
 ProcessFinished (virtual protected)

	Returns kJTrue if document should stay open.

 ******************************************************************************/

JBoolean
CBExecOutputDocument::ProcessFinished
	(
	const JProcess::Finished& info
	)
{
	// Since the process has terminated, we know that it has written
	// everything to the pipe.  When handle_input() no longer broadcasts,
	// we know we have read everything.

	do
		{
		itsReceivedDataFlag = kJFalse;
		JXApplication::CheckACEReactor();
		}
		while (itsReceivedDataFlag);

	const pid_t pid = itsProcess->GetPID();
	JProcess* p = itsProcess;
	itsProcess = NULL;
	jdelete p;

	delete itsRecordLink;
	itsRecordLink = NULL;

	delete itsDataLink;
	itsDataLink = NULL;

	CloseOutFD();
	UpdateButtons();

	CBTextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	te->SetCaretLocation(te->GetTextLength()+1);

	JBoolean stayOpen = kJTrue;

	int result;
	const JChildExitReason reason = info.GetReason(&result);

	if (info.Successful() ||
		(reason == kJChildSignalled && result == SIGKILL))
		{
		DataReverted();
		te->ClearUndo();
		if (!IsActive())
			{
			stayOpen = kJFalse;
			}
		else
			{
			const JString reasonStr = JPrintChildExitReason(reason, result);

			te->Paste("\n");
			te->Paste(reasonStr);
			te->Paste("\n");

			DataReverted();
			te->ClearUndo();
			}
		}
	else
		{
		const JString reasonStr = JPrintChildExitReason(reason, result);

		te->Paste("\n");
		te->Paste(reasonStr);
		te->Paste("\n");

		if (reason != kJChildFinished)
			{
			#ifdef _J_OSX
			const JCharacter* path = "/cores/";
			#else
			const JCharacter* path = itsPath;
			#endif

			const JString coreName = "core." + JString(pid, JString::kBase10);
			const JString coreFull = JCombinePathAndName(path, coreName);
			if (JFileExists(coreFull))
				{
				te->Paste(JGetString("CoreLocation::CBExecOutputDocument"));
				te->SetCurrentFontBold(kJTrue);
				te->Paste(coreFull);
				te->SetCurrentFontBold(kJFalse);
				}
			else
				{
				const JCharacter* map[] =
					{
					"name", coreName
					};
				const JString msg = JGetString("CoreName::CBExecOutputDocument", map, sizeof(map));
				te->Paste(msg);
				}
			te->Paste("\n");
			}

		DataReverted();
		te->ClearUndo();
		if (!IsActive() || GetWindow()->IsIconified())	// don't raise if active
			{
			Activate();
			}
		GetDisplay()->Beep();
		}

	if (state.hadSelection)
		{
		te->RestoreDisplayState(state);
		}

	return stayOpen;
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CBExecOutputDocument::UpdateButtons()
{
	if (ProcessRunning())
		{
		itsPauseButton->Activate();
		itsKillButton->Activate();
		if (itsStopButton != NULL)
			{
			itsStopButton->Activate();
			}
		itsCmdPrompt->Show();
		itsCmdInput->Show();
		itsEOFButton->Show();
		SetFileDisplayVisible(kJFalse);

		if (itsProcessPausedFlag)
			{
			itsPauseButton->SetLabel(JGetString("ResumeLabel::CBExecOutputDocument"));
			}
		else
			{
			itsPauseButton->SetLabel(JGetString("PauseLabel::CBExecOutputDocument"));
			}

		if (!itsProcessPausedFlag && itsCmdStream != NULL)
			{
			itsCmdInput->Activate();
			itsEOFButton->Activate();
			if (itsFocusToCmdFlag)
				{
				itsCmdInput->Focus();
				}
			}
		else if (itsCmdStream == NULL)
			{
			itsCmdPrompt->Hide();
			itsCmdInput->Hide();
			itsEOFButton->Hide();
			GetTextEditor()->Focus();
			}
		else
			{
			itsCmdInput->Deactivate();
			itsEOFButton->Deactivate();
			GetTextEditor()->Focus();
			}
		}
	else
		{
		itsPauseButton->Deactivate();
		itsKillButton->Deactivate();
		if (itsStopButton != NULL)
			{
			itsStopButton->Deactivate();
			}
		itsCmdPrompt->Hide();
		itsCmdInput->Hide();
		itsEOFButton->Hide();
		SetFileDisplayVisible(kJTrue);
		GetTextEditor()->Focus();

		itsPauseButton->SetLabel(JGetString("PauseLabel::CBExecOutputDocument"));
		}
}

/******************************************************************************
 OpenPrevListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBExecOutputDocument::OpenPrevListItem()
{
	assert(0);
}

/******************************************************************************
 OpenNextListItem (virtual)

	Derived classes which claim to manage a list of files must override this.

 ******************************************************************************/

void
CBExecOutputDocument::OpenNextListItem()
{
	assert(0);
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Check if the file is relative to the exec path.

 ******************************************************************************/

void
CBExecOutputDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	if (JIsAbsolutePath(*fileName))
		{
		return;
		}

	const JString testName = JCombinePathAndName(itsPath, *fileName);
	if (JFileExists(testName))
		{
		*fileName = testName;
		}
	else
		{
		CBTextDocument::ConvertSelectionToFullPath(fileName);
		}
}

/******************************************************************************
 NeedsFormattedData (virtual protected)

	Derived classes should override to return kJTrue if they need to use
	RecordLink.

 ******************************************************************************/

JBoolean
CBExecOutputDocument::NeedsFormattedData()
	const
{
	return kJFalse;
}
