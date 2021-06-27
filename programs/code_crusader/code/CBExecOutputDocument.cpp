/******************************************************************************
 CBExecOutputDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "CBExecOutputDocument.h"
#include "CBTextEditor.h"
#include "CBCmdLineInput.h"
#include "CBExecOutputPostFTCTask.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JOutPipeStream.h>
#include <JStringIterator.h>
#include <jTextUtil.h>
#include <jDirUtil.h>
#include <jSignal.h>
#include <jTime.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JSize kMenuButtonWidth = 60;

static const JString kNewLine("\n", kJFalse);

// JBroadcaster message types

const JUtf8Byte* CBExecOutputDocument::kFinished = "Finished::CBExecOutputDocument";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExecOutputDocument::CBExecOutputDocument
	(
	const CBTextFileType	fileType,
	const JUtf8Byte*		helpSectionName,
	const JBoolean			focusToCmd,
	const JBoolean			allowStop
	)
	:
	CBTextDocument(fileType, helpSectionName, kJFalse),
	itsFocusToCmdFlag(focusToCmd)
{
	itsProcess            = nullptr;
	itsRecordLink         = nullptr;
	itsDataLink           = nullptr;
	itsCmdStream          = nullptr;
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
	assert( itsPauseButton != nullptr );
	ListenTo(itsPauseButton);
	itsPauseButton->SetShortcuts(JString("^Z", kJFalse));
	itsPauseButton->SetHint(JGetString("PauseButtonHint::CBExecOutputDocument"));

	if (allowStop)
		{
		itsStopButton =
			jnew JXTextButton(JGetString("StopLabel::CBExecOutputDocument"), window,
							  JXWidget::kFixedRight, JXWidget::kFixedTop,
							  rect.right - 2*kMenuButtonWidth,0, kMenuButtonWidth,h);
		assert( itsStopButton != nullptr );
		ListenTo(itsStopButton);
		itsStopButton->SetShortcuts(JString("^C#.", kJFalse));
		itsStopButton->SetHint(JGetString("StopButtonHint::CBExecOutputDocument"));
		}
	else
		{
		itsStopButton = nullptr;
		}

	itsKillButton =
		jnew JXTextButton(JGetString("KillLabel::CBExecOutputDocument"), window,
						  JXWidget::kFixedRight, JXWidget::kFixedTop,
						  rect.right - kMenuButtonWidth,0, kMenuButtonWidth,h);
	assert( itsKillButton != nullptr );
	ListenTo(itsKillButton);

	if (!allowStop)
		{
		itsKillButton->SetShortcuts(JString("^C#.", kJFalse));
		itsKillButton->SetHint(JGetString("StopButtonHint::CBExecOutputDocument"));
		}

	menuBar->AdjustSize(-x, 0);

	// command line input

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	GetFileDisplayInfo(&hSizing, &vSizing);

	itsCmdPrompt =
		jnew JXStaticText(JGetString("CmdPrompt::CBExecOutputDocument"), window,
						  JXWidget::kFixedLeft, vSizing,
						  -1000, -1000, 0, 500);
	assert( itsCmdPrompt != nullptr );
	itsCmdPrompt->SetToLabel();
	itsCmdPrompt->Hide();

	itsCmdInput =
		jnew CBCmdLineInput(this, window, hSizing, vSizing,
							-1000, -1000, 500, 500);
	assert( itsCmdInput != nullptr );
	itsCmdInput->ShareEditMenu(GetTextEditor());
	itsCmdInput->Hide();

	itsEOFButton =
		jnew JXTextButton(JGetString("EOFButtonTitle::CBExecOutputDocument"), window,
						  JXWidget::kFixedRight, vSizing,
						  -1000, -1000, 500, 500);
	assert( itsEOFButton != nullptr );
	itsEOFButton->SetShortcuts(JString("^D", kJFalse));
	itsEOFButton->Hide();
	ListenTo(itsEOFButton);

	JXUrgentTask* task = jnew CBExecOutputPostFTCTask(this);
	assert( task != nullptr );
	task->Go();

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
	itsRecordLink = nullptr;

	delete itsDataLink;
	itsDataLink = nullptr;

	CloseOutFD();

	jdelete itsProcess;
}

/******************************************************************************
 PlaceCmdLineWidgets (virtual protected)

 ******************************************************************************/

const JCoordinate kMinCmdInputWidth = 20;

void
CBExecOutputDocument::PlaceCmdLineWidgets()
{
	JXWindow* window = GetWindow();

	JXWidget::HSizingOption hSizing;
	JXWidget::VSizingOption vSizing;
	const JRect fileRect = GetFileDisplayInfo(&hSizing, &vSizing);

	itsCmdPrompt->Place(fileRect.left, fileRect.top);
	itsCmdPrompt->AdjustSize(0, fileRect.height() - itsCmdPrompt->GetFrameHeight());

	const JCoordinate promptWidth = itsCmdPrompt->GetFrameWidth();

	const JCoordinate eofWidth =
		itsEOFButton->GetFont().GetStringWidth(GetDisplay()->GetFontManager(), itsEOFButton->GetLabel()) +
		2 * itsEOFButton->GetPadding().x +
		2 * itsEOFButton->GetBorderWidth();

	itsCmdInput->Place(fileRect.left + promptWidth, fileRect.top);

	JCoordinate cmdInputWidth = fileRect.width() - promptWidth - eofWidth;
	if (cmdInputWidth < kMinCmdInputWidth)
		{
		window->AdjustSize(kMinCmdInputWidth - cmdInputWidth, 0);
		cmdInputWidth = kMinCmdInputWidth;
		}
	itsCmdInput->SetSize(cmdInputWidth, fileRect.height());

	const JPoint p = window->GetMinSize();
	window->SetMinSize(window->GetFrameWidth() - (cmdInputWidth - kMinCmdInputWidth), p.y);

	itsEOFButton->Place(fileRect.right - eofWidth, fileRect.top);
	itsEOFButton->SetSize(eofWidth, fileRect.height());

	UpdateButtons();
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
	if (itsUseCount > 0 || itsRecordLink != nullptr || itsDataLink != nullptr)
		{
		Activate();
		JGetUserNotification()->ReportError(itsDontCloseMsg);
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

	execCmd can be nullptr.  If it is, the header (timestamp + path + cmd)
	is not printed.

 ******************************************************************************/

void
CBExecOutputDocument::SetConnection
	(
	JProcess*		p,
	const int		inFD,
	const int		outFD,
	const JString&	windowTitle,
	const JString&	dontCloseMsg,
	const JString&	execDir,
	const JString&	execCmd,
	const JBoolean	showPID
	)
{
	assert( !ProcessRunning() && itsRecordLink == nullptr && itsDataLink == nullptr );

	itsProcess = p;
	ListenTo(itsProcess);

	if (NeedsFormattedData())
		{
		itsRecordLink = new RecordLink(inFD);
		assert( itsRecordLink != nullptr );
		ListenTo(itsRecordLink);
		}
	else
		{
		itsDataLink = new DataLink(inFD);
		assert( itsDataLink != nullptr );
		ListenTo(itsDataLink);
		}

	if (outFD != ACE_INVALID_HANDLE)
		{
		itsCmdStream = jnew JOutPipeStream(outFD, kJTrue);
		assert( itsCmdStream != nullptr );
		}

	CBTextEditor* te = GetTextEditor();
	if (itsClearWhenStartFlag)
		{
		te->GetText()->SetText(JString::empty);
		}
	else if (!te->GetText()->IsEmpty())
		{
		const JString& text = te->GetText()->GetText();
		JStringIterator iter(text, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c, kJFalse) && c == '\n')
			{
			iter.SkipPrev();
			}

		if (!iter.AtEnd())
			{
			te->SetSelection(JCharacterRange(
				iter.GetNextCharacterIndex(), text.GetCharacterCount()));
			}

		te->Paste(JString("\n\n----------\n\n", kJFalse));
		te->GetText()->ClearUndo();
		}

	if (!execCmd.IsEmpty())
		{
		const JString timeStamp = JGetTimeStamp();

		te->Paste(timeStamp);
		te->Paste(kNewLine);
		te->Paste(execDir);
		te->Paste(kNewLine);
		te->Paste(execCmd);

		if (showPID)
			{
			te->Paste(kNewLine);
			te->Paste(JGetString("ProcessID::CBExecOutputDocument"));
			te->Paste(JString((JUInt64) p->GetPID()));
			}

		te->Paste(kNewLine);
		te->Paste(kNewLine);
		te->GetText()->ClearUndo();
		}

	itsPath              = execDir;
	itsReceivedDataFlag  = kJFalse;
	itsProcessPausedFlag = kJFalse;
	itsDontCloseMsg      = dontCloseMsg;
	FileChanged(windowTitle, kJFalse);

	UpdateButtons();
	te->SetWritable(kJFalse);
	itsCmdInput->GetText()->SetText(JString::empty);
}

/******************************************************************************
 SendText

 ******************************************************************************/

void
CBExecOutputDocument::SendText
	(
	const JString& text
	)
{
	if (ProcessRunning() && itsCmdStream != nullptr)
		{
		text.Print(*itsCmdStream);
		itsCmdStream->flush();

		CBTextEditor* te = GetTextEditor();
		te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount() + 1);

		if (text.GetFirstCharacter() != '\0' && text.GetFirstCharacter() != '\n' &&
			itsRecordLink != nullptr &&
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
		assert( info != nullptr );
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
		if (!itsCmdInput->GetText()->IsEmpty())
			{
			itsCmdInput->HandleKeyPress(
				JUtf8Character(kJReturnKey), 0, JXKeyModifiers(GetDisplay()));
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
	assert( itsRecordLink != nullptr );

	JString text;
	const JBoolean ok = itsRecordLink->GetNextMessage(&text);
	assert( ok );

	// remove text that has already been printed

	if (!itsLastPrompt.IsEmpty() && text.BeginsWith(itsLastPrompt))
		{
		JStringIterator iter(&text, kJIteratorStartAfter, itsLastPrompt.GetCharacterCount());
		iter.RemoveAllPrev();
		}
	itsLastPrompt.Clear();

	const JXTEBase::DisplayState state = GetTextEditor()->SaveDisplayState();

	AppendText(text);
	GetTextEditor()->GetText()->ClearUndo();

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
	JPasteUNIXTerminalOutput(text, te->GetText()->GetBeyondEnd(), te->GetText());
	te->Paste(kNewLine);
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
	assert( info != nullptr );

	CBTextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	JPasteUNIXTerminalOutput(info->GetData(), te->GetText()->GetBeyondEnd(), te->GetText());
	te->GetText()->ClearUndo();

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
	if (itsCmdStream != nullptr)
		{
		jdelete itsCmdStream;
		itsCmdStream = nullptr;
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
	itsProcess = nullptr;
	jdelete p;

	delete itsRecordLink;
	itsRecordLink = nullptr;

	delete itsDataLink;
	itsDataLink = nullptr;

	CloseOutFD();
	UpdateButtons();

	CBTextEditor* te                   = GetTextEditor();
	const JXTEBase::DisplayState state = te->SaveDisplayState();

	te->SetCaretLocation(te->GetText()->GetText().GetCharacterCount()+1);

	JBoolean stayOpen = kJTrue;

	int result;
	const JChildExitReason reason = info.GetReason(&result);

	if (info.Successful() ||
		(reason == kJChildSignalled && result == SIGKILL))
		{
		DataReverted();
		te->GetText()->ClearUndo();
		if (!IsActive())
			{
			stayOpen = kJFalse;
			}
		else
			{
			const JString reasonStr = JPrintChildExitReason(reason, result);

			te->Paste(kNewLine);
			te->Paste(reasonStr);
			te->Paste(kNewLine);

			DataReverted();
			te->GetText()->ClearUndo();
			}
		}
	else
		{
		const JString reasonStr = JPrintChildExitReason(reason, result);

		te->Paste(kNewLine);
		te->Paste(reasonStr);
		te->Paste(kNewLine);

		if (reason != kJChildFinished)
			{
			#ifdef _J_OSX
			const JString path("/cores/", kJFalse);
			#else
			const JString path = itsPath;
			#endif

			const JString coreName = "core." + JString((JUInt64) pid);
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
				const JUtf8Byte* map[] =
					{
					"name", coreName.GetBytes()
					};
				const JString msg = JGetString("CoreName::CBExecOutputDocument", map, sizeof(map));
				te->Paste(msg);
				}
			te->Paste(kNewLine);
			}

		DataReverted();
		te->GetText()->ClearUndo();
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
		if (itsStopButton != nullptr)
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

		if (!itsProcessPausedFlag && itsCmdStream != nullptr)
			{
			itsCmdInput->Activate();
			itsEOFButton->Activate();
			if (itsFocusToCmdFlag)
				{
				itsCmdInput->Focus();
				}
			}
		else if (itsCmdStream == nullptr)
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
		if (itsStopButton != nullptr)
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
