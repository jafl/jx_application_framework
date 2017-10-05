/******************************************************************************
 JXFSRunFileDialog.cpp.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by John Lindal. All rights reserved.

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

static const JCharacter* kHelpSectionName = "JFSRunFileHelp";

// setup information

static const JCharacter* kPrefsFileRoot = "jx/jfs/run_file_dialog";
const JFileVersion kCurrentPrefsVersion = 0;

// string ID's

static const JCharacter* kRunWithFileID  = "RunWithFile::JXFSRunFileDialog";
static const JCharacter* kRunWithFilesID = "RunWithFiles::JXFSRunFileDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunFileDialog::JXFSRunFileDialog
	(
	const JCharacter*	fileName,
	const JBoolean		allowSaveCmd
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
		itsCmdHistoryMenu->AddCommand(itsCmdInput->GetText(),
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

	return itsCmdInput->GetText();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXFSRunFileDialog::BuildWindow
	(
	const JCharacter*	fileName,
	const JBoolean		allowSaveCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 450,170, "");
	assert( window != NULL );

	JXWidgetSet* ftcEnclosure =
		jnew JXWidgetSet(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 180,90, 250,40);
	assert( ftcEnclosure != NULL );

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 300,20);
	assert( itsCmdInput != NULL );

	JXStaticText* prompt =
		jnew JXStaticText(JGetString("prompt::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 300,20);
	assert( prompt != NULL );
	prompt->SetToLabel();

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXFSRunFileDialog::JXLayout"), ftcEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSRunFileDialog::JXLayout"), ftcEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsOKButton =
		jnew JXTextButton(JGetString("itsOKButton::JXFSRunFileDialog::JXLayout"), ftcEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 189,10, 60,20);
	assert( itsOKButton != NULL );
	itsOKButton->SetShortcuts(JGetString("itsOKButton::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsCmdHistoryMenu =
		jnew JXFSCommandHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,40, 30,20);
	assert( itsCmdHistoryMenu != NULL );

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,40, 60,20);
	assert( itsChooseCmdButton != NULL );

	itsSaveBindingCB =
		jnew JXTextCheckbox(JGetString("itsSaveBindingCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,140, 270,20);
	assert( itsSaveBindingCB != NULL );
	itsSaveBindingCB->SetShortcuts(JGetString("itsSaveBindingCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 130,20);
	assert( itsUseShellCB != NULL );
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 130,20);
	assert( itsUseWindowCB != NULL );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::JXFSRunFileDialog::shortcuts::JXLayout"));

	JXStaticText* cmdHint =
		jnew JXStaticText(JGetString("cmdHint::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,60, 340,20);
	assert( cmdHint != NULL );
	cmdHint->SetFontSize(8);
	cmdHint->SetToLabel();

	itsSingleFileCB =
		jnew JXTextCheckbox(JGetString("itsSingleFileCB::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 130,20);
	assert( itsSingleFileCB != NULL );
	itsSingleFileCB->SetShortcuts(JGetString("itsSingleFileCB::JXFSRunFileDialog::shortcuts::JXLayout"));

// end JXLayout

	SetButtons(itsOKButton, cancelButton);

	window->SetTitle("Run command with file");
	window->LockCurrentMinSize();
	UseModalPlacement(kJFalse);

	ListenTo(itsChooseCmdButton);
	ListenTo(itsHelpButton);
	ListenTo(itsCmdHistoryMenu);

	itsCmdInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(window->GetFontManager()->GetDefaultMonospaceFont());
	itsCmdHistoryMenu->SetDefaultFont(window->GetFontManager()->GetDefaultMonospaceFont(), kJTrue);

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
		const JCharacter* map[] =
			{
			"name", name.GetCString()
			};
		promptStr = JGetString(kRunWithFileID, map, sizeof(map));
		}
	else
		{
		promptStr = JGetString(kRunWithFilesID);
		}
	prompt->SetText(promptStr);

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
	if (itsCmdInput->IsEmpty())
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
		(JXGetHelpManager())->ShowSection(kHelpSectionName);
		}

	else if (sender == itsCmdInput &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
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

	singleFileCB can be NULL

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
	cmdInput->SetText(cmd);
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

	if (singleFileCB != NULL)
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
	if ((JGetChooseSaveFile())->ChooseFile("Select program", "", &fullName))
		{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);
		name.AppendCharacter(' ');
		cmdInput->SetText(name);
		cmdInput->SetCaretLocation(name.GetLength() + 1);
		}
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

JBoolean
JXFSRunFileDialog::ReadSetup()
{
	JBoolean found = kJFalse;

	JPrefsFile* file = NULL;
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
	JPrefsFile* file = NULL;
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
	itsCmdInput->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> checked;
	itsUseShellCB->SetState(checked);

	input >> checked;
	itsUseWindowCB->SetState(checked);

	input >> checked;
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

	output << ' ' << itsCmdInput->GetText();

	output << ' ';
	itsCmdHistoryMenu->WriteSetup(output);

	output << ' ' << itsUseShellCB->IsChecked();
	output << ' ' << itsUseWindowCB->IsChecked();
	output << ' ' << itsSingleFileCB->IsChecked();
}
