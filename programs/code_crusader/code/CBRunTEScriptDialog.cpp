/******************************************************************************
 CBRunTEScriptDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBRunTEScriptDialog.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXStringHistoryMenu.h>
#include <JXStaticText.h>
#include <JXDocumentMenu.h>
#include <JXHelpManager.h>
#include <JXChooseSaveFile.h>
#include <JOutPipeStream.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <JString.h>
#include <JSubstitute.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 stores itsStayOpenCB

/******************************************************************************
 Constructor

 ******************************************************************************/

CBRunTEScriptDialog::CBRunTEScriptDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBRunTEScriptDialogID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRunTEScriptDialog::~CBRunTEScriptDialog()
{
	// prefs written by CBDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBRunTEScriptDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive())
		{
		itsCmdInput->Focus();
		itsCmdInput->SelectAll();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBRunTEScriptDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,110, JString::empty);
	assert( window != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::CBRunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,80, 60,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::CBRunTEScriptDialog::shortcuts::JXLayout"));

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::CBRunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,80, 60,20);
	assert( itsRunButton != nullptr );
	itsRunButton->SetShortcuts(JGetString("itsRunButton::CBRunTEScriptDialog::shortcuts::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 320,20);
	assert( itsCmdInput != nullptr );

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::CBRunTEScriptDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 350,20);
	assert( cmdLabel != nullptr );
	cmdLabel->SetToLabel();

	itsHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,40, 30,20);
	assert( itsHistoryMenu != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBRunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,80, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBRunTEScriptDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::CBRunTEScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( itsStayOpenCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBRunTEScriptDialog"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsRunButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsHistoryMenu);

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsStayOpenCB->SetState(true);

	// create hidden JXDocument so Meta-# shortcuts work

	auto* fileListMenu =
		jnew JXDocumentMenu(JString::empty, window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != nullptr );

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBRunTEScriptDialog::UpdateDisplay()
{
	if (itsCmdInput->GetText()->IsEmpty())
		{
		itsRunButton->Deactivate();
		}
	else
		{
		itsRunButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBRunTEScriptDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRunButton && message.Is(JXButton::kPushed))
		{
		RunScript();
		if (!itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBEditorHelp-ExtScript");
		}

	else if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		itsHistoryMenu->UpdateInputField(message, itsCmdInput);
		itsCmdInput->Focus();
		}

	else if (sender == itsCmdInput &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 RunSimpleScript

	Runs the given script with argyments:  path file_name line_number

 ******************************************************************************/

bool
CBRunTEScriptDialog::RunSimpleScript
	(
	const JString&	scriptName,
	JTextEditor*	te,
	const JString&	fileName
	)
{
	JString cmd = JPrepArgForExec(scriptName);
	cmd        += " $p $n $l";
	return RunScript(cmd, te, fileName);
}

/******************************************************************************
 RunScript

 ******************************************************************************/

bool
CBRunTEScriptDialog::RunScript()
{
	CBTextDocument* textDoc = nullptr;
	if (CBGetDocumentManager()->GetActiveTextDocument(&textDoc))
		{
		bool onDisk;
		const JString fullName = textDoc->GetFullName(&onDisk);
		if (onDisk)
			{
			const bool result = RunScript(textDoc->GetTextEditor(), fullName);
			textDoc->Activate();
			return result;
			}
		else
			{
			JGetUserNotification()->ReportError(
				JGetString("FileDoesNotExist::CBRunTEScriptDialog"));
			return false;
			}
		}
	else
		{
		JGetUserNotification()->ReportError(
			JGetString("NoEditor::CBRunTEScriptDialog"));
		return false;
		}
}

// private

bool
CBRunTEScriptDialog::RunScript
	(
	JTextEditor*	te,
	const JString&	fullName
	)
{
	if (!itsCmdInput->InputValid())
		{
		return false;
		}

	itsCmdInput->GetText()->DeactivateCurrentUndo();

	if (RunScript(itsCmdInput->GetText()->GetText(), te, fullName))
		{
		itsHistoryMenu->AddString(itsCmdInput->GetText()->GetText());
		return true;
		}
	else
		{
		return false;
		}
}

bool
CBRunTEScriptDialog::RunScript
	(
	const JString&	origCmd,
	JTextEditor*	te,
	const JString&	fullName
	)
{
	JString cmd = origCmd;
	ReplaceVariables(&cmd, te, fullName);

	pid_t pid;
	int toFD, fromFD, errFD;
	const JError execErr =
		JExecute(cmd, &pid, kJCreatePipe, &toFD,
				 kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
	if (!execErr.OK())
		{
		Activate();
		execErr.ReportIfError();
		return false;
		}
	else
		{
		JOutPipeStream output(toFD, true);
		JString text;
		if (te->GetSelection(&text))
			{
			text.Print(output);
			}
		output.close();

		// read stdout first since this is more likely to fill up

		JReadAll(fromFD, &text);

		JString msg;
		JReadAll(errFD, &msg);
		if (!msg.IsEmpty())
			{
			msg.Prepend(JGetString("Error::CBGlobal"));
			JGetUserNotification()->ReportError(msg);
			return false;
			}

		text.TrimWhitespace();
		if (!text.IsEmpty())
			{
			te->Paste(text);
			}
		}

	return true;
}

/******************************************************************************
 ReplaceVariables (private)

 ******************************************************************************/

void
CBRunTEScriptDialog::ReplaceVariables
	(
	JString*		cmd,
	JTextEditor*	te,
	const JString&	fullName
	)
{
	JString path, fileName;
	JSplitPathAndName(fullName, &path, &fileName);

	const JString lineIndexStr(
		te->VisualLineIndexToCRLineIndex(
			te->GetLineForChar(te->GetInsertionCharIndex())),
		0);

	JSubstitute sub;
	sub.IgnoreUnrecognized();
	sub.DefineVariable("f", fullName);
	sub.DefineVariable("p", path);
	sub.DefineVariable("n", fileName);
	sub.DefineVariable("l", lineIndexStr);
	sub.Substitute(cmd);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBRunTEScriptDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsHistoryMenu->ReadSetup(input);

	if (vers >= 1)
		{
		bool stayOpen;
		input >> JBoolFromString(stayOpen);
		itsStayOpenCB->SetState(stayOpen);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBRunTEScriptDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsStayOpenCB->IsChecked());
	output << ' ';
}
