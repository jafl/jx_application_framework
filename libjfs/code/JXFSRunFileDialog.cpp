/******************************************************************************
 JXFSRunFileDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXFSRunFileDialog.h"
#include "JXFSCommandHistoryMenu.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXWidgetSet.h>
#include <JXChooseSaveFile.h>
#include <JXFontManager.h>
#include <jXGlobals.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jTime.h>
#include <sstream>
#include <jAssert.h>

const JSize kHistoryLength = 20;

// setup information

static const JString kPrefsFileRoot("jx/jfs/run_file_dialog", kJFalse);
const JFileVersion kCurrentPrefsVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunFileDialog::JXFSRunFileDialog
	(
	const JString&	fileName,
	const JBoolean	allowSaveCmd
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(fileName, allowSaveCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSRunFileDialog::~JXFSRunFileDialog()
{
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXFSRunFileDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		WriteSetup();	// here because we are invoked again before dtor
		return kJTrue;
		}
	else
		{
		itsCmdHistoryMenu->AddCommand(itsCmdInput->GetText()->GetText(),
									  itsUseShellCB->IsChecked(),
									  itsUseWindowCB->IsChecked(),
									  itsSingleFileCB->IsChecked());

		WriteSetup();	// here because we are invoked again before dtor
		return kJTrue;
		}
}

/******************************************************************************
 GetCommand

 ******************************************************************************/

const JString&
JXFSRunFileDialog::GetCommand
	(
	JFSBinding::CommandType*	type,
	JBoolean*					singleFile,
	JBoolean*					saveBinding
	)
	const
{
	*type = JFSBinding::GetCommandType(itsUseShellCB->IsChecked(),
									   itsUseWindowCB->IsChecked());

	*singleFile  = itsSingleFileCB->IsChecked();
	*saveBinding = itsSaveBindingCB->IsChecked();

	return itsCmdInput->GetText()->GetText();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXFSRunFileDialog::BuildWindow
	(
	const JString&	fileName,
	const JBoolean	allowSaveCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 450,170, JString::empty);
	assert( window != nullptr );

	JXWidgetSet* ftcContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 180,90, 250,40);
	assert( ftcContainer != nullptr );

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 300,20);
	assert( itsCmdInput != nullptr );

	JXStaticText* prompt =
		jnew JXStaticText(JGetString("prompt::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 300,20);
	assert( prompt != nullptr );
	prompt->SetToLabel();

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXFSRunFileDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 60,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSRunFileDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsOKButton =
		jnew JXTextButton(JGetString("itsOKButton::JXFSRunFileDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 189,10, 60,20);
	assert( itsOKButton != nullptr );
	itsOKButton->SetShortcuts(JGetString("itsOKButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsCmdHistoryMenu =
		jnew JXFSCommandHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,40, 30,20);
	assert( itsCmdHistoryMenu != nullptr );

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,40, 60,20);
	assert( itsChooseCmdButton != nullptr );

	itsSaveBindingCB =
		jnew JXTextCheckbox(JGetString("itsSaveBindingCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,140, 270,20);
	assert( itsSaveBindingCB != nullptr );
	itsSaveBindingCB->SetShortcuts(JGetString("itsSaveBindingCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 130,20);
	assert( itsUseShellCB != nullptr );
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 130,20);
	assert( itsUseWindowCB != nullptr );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	JXStaticText* cmdHint =
		jnew JXStaticText(JGetString("cmdHint::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 20,60, 340,20);
	assert( cmdHint != nullptr );
	cmdHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	cmdHint->SetToLabel();

	itsSingleFileCB =
		jnew JXTextCheckbox(JGetString("itsSingleFileCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 130,20);
	assert( itsSingleFileCB != nullptr );
	itsSingleFileCB->SetShortcuts(JGetString("itsSingleFileCB::JXFSRunFileDialog::shortcuts::JXLayout"));

// end JXLayout

	SetButtons(itsOKButton, cancelButton);

	window->SetTitle(JGetString("WindowTitle::JXFSRunFileDialog"));
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);
	ftcContainer->SetNeedsInternalFTC();

	ListenTo(itsChooseCmdButton);
	ListenTo(itsHelpButton);
	ListenTo(itsCmdHistoryMenu);

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsCmdHistoryMenu->SetDefaultFont(JFontManager::GetDefaultMonospaceFont(), kJTrue);

	// check for suffix

	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	JString root, suffix;
	if (!allowSaveCmd || !JSplitRootAndSuffix(name, &root, &suffix))
		{
		itsSaveBindingCB->Hide();
		}

	// set prompt

	JString promptStr;
	if (allowSaveCmd)
		{
		const JUtf8Byte* map[] =
			{
			"name", name.GetBytes()
			};
		promptStr = JGetString("RunWithFile::JXFSRunFileDialog", map, sizeof(map));
		}
	else
		{
		promptStr = JGetString("RunWithFiles::JXFSRunFileDialog");
		}
	prompt->GetText()->SetText(promptStr);

	// read previous window geometry

	if (!ReadSetup())
		{
		window->PlaceAsDialogWindow();
		}

	// adjust window width to fit prompt

	prompt->SetBreakCROnly(kJTrue);
	const JSize prefw = prompt->TEGetMinPreferredGUIWidth() + 10;
	const JSize apw   = prompt->GetApertureWidth();
	if (prefw > apw)
		{
		GetWindow()->AdjustSize(prefw - apw, 0);
		}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXFSRunFileDialog::UpdateDisplay()
{
	if (itsCmdInput->GetText()->IsEmpty())
		{
		itsOKButton->Deactivate();
		}
	else
		{
		itsOKButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSRunFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCmdHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		HandleHistoryMenu(message, itsCmdHistoryMenu, itsCmdInput,
						  itsUseShellCB, itsUseWindowCB, itsSingleFileCB);
		}
	else if (sender == itsChooseCmdButton && message.Is(JXButton::kPushed))
		{
		HandleChooseCmdButton(itsCmdInput);
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(JGetString("HelpLink::JXFSRunFileDialog").GetBytes());
		}

	else if (sender == itsCmdInput &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 HandleHistoryMenu (static)

	singleFileCB can be nullptr

 ******************************************************************************/

void
JXFSRunFileDialog::HandleHistoryMenu
	(
	const Message&			message,
	JXFSCommandHistoryMenu*	menu,
	JXInputField*			cmdInput,
	JXTextCheckbox*			shellCB,
	JXTextCheckbox*			windowCB,
	JXTextCheckbox*			singleFileCB
	)
{
	JFSBinding::CommandType type;
	JBoolean singleFile;
	const JString& cmd = menu->GetCommand(message, &type, &singleFile);

	cmdInput->Focus();
	cmdInput->GetText()->SetText(cmd);
	cmdInput->SelectAll();

	shellCB->SetState(kJFalse);
	windowCB->SetState(kJFalse);
	if (type == JFSBinding::kRunInShell)
		{
		shellCB->SetState(kJTrue);
		}
	else if (type == JFSBinding::kRunInWindow)
		{
		windowCB->SetState(kJTrue);
		}

	if (singleFileCB != nullptr)
		{
		singleFileCB->SetState(singleFile);
		}
}

/******************************************************************************
 HandleChooseCmdButton (static)

 ******************************************************************************/

void
JXFSRunFileDialog::HandleChooseCmdButton
	(
	JXInputField* cmdInput
	)
{
	JString fullName;
	if (JGetChooseSaveFile()->ChooseFile(
			JGetString("ChooseCmdPrompt::JXFSRunFileDialog"),
			JString::empty, &fullName))
		{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);
		name.Append(" ");
		cmdInput->GetText()->SetText(name);
		cmdInput->GoToEndOfLine();
		}
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

JBoolean
JXFSRunFileDialog::ReadSetup()
{
	JBoolean found = kJFalse;

	JPrefsFile* file = nullptr;
	if ((JPrefsFile::Create(kPrefsFileRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
		{
		for (JFileVersion vers = kCurrentPrefsVersion; kJTrue; vers--)
			{
			if (file->IDValid(vers))
				{
				std::string data;
				file->GetData(vers, &data);
				std::istringstream input(data);
				ReadSetup(input);
				found = kJTrue;
				break;
				}

			if (vers == 0)
				{
				break;	// check *before* decrement since unsigned
				}
			}

		jdelete file;
		}

	return found;
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
JXFSRunFileDialog::WriteSetup()
{
	JPrefsFile* file = nullptr;
	if ((JPrefsFile::Create(kPrefsFileRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
		{
		std::ostringstream data;
		WriteSetup(data);
		file->SetData(kCurrentPrefsVersion, data);
		jdelete file;
		}
}

/******************************************************************************
 ReadSetup (private)

	ReadSetup() guarantees readable version

 ******************************************************************************/

void
JXFSRunFileDialog::ReadSetup
	(
	std::istream& input
	)
{
	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	JString s;
	JBoolean checked;

	input >> s;
	itsCmdInput->GetText()->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> JBoolFromString(checked);
	itsUseShellCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsUseWindowCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsSingleFileCB->SetState(checked);
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
JXFSRunFileDialog::WriteSetup
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsCmdInput->GetText()->GetText();

	output << ' ';
	itsCmdHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsUseShellCB->IsChecked())
				  << JBoolToString(itsUseWindowCB->IsChecked())
				  << JBoolToString(itsSingleFileCB->IsChecked());
}
