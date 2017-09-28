/******************************************************************************
 JXFSRunCommandDialog.cpp.cc

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXFSRunCommandDialog.h"
#include "JXFSCommandHistoryMenu.h"
#include "JXFSBindingManager.h"
#include "JXFSRunFileDialog.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXPathInput.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXPathHistoryMenu.h>
#include <JXFlatRect.h>
#include <JXChooseSaveFile.h>
#include <JXCheckModTimeTask.h>
#include <JXFontManager.h>
#include <jXGlobals.h>
#include <JSimpleProcess.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <sstream>
#include <jAssert.h>

const JSize kHistoryLength = 20;

static const JCharacter* kHelpSectionName = "JFSRunCommandHelp";

// setup information

static const JCharacter* kPrefsFileRoot = "jx/jfs/run_cmd_dialog";
const JFileVersion kCurrentPrefsVersion = 0;

static const JCharacter* kSignalFileName = "~/.jx/jfs/run_cmd_dialog.signal";
const Time kUpdateInterval               = 1000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunCommandDialog::JXFSRunCommandDialog()
	:
	JXWindowDirector(JXGetPersistentWindowOwner())
{
	JXFSBindingManager::Initialize();	// create directory for prefs

	JString signalFileName;
	if (JExpandHomeDirShortcut(kSignalFileName, &signalFileName))
		{
		itsSignalTask = jnew JXCheckModTimeTask(kUpdateInterval, signalFileName);
		assert( itsSignalTask != NULL );
		itsSignalTask->Start();
		ListenTo(itsSignalTask);
		}

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSRunCommandDialog::~JXFSRunCommandDialog()
{
	WriteSetup();	// save "stay open" state

	jdelete itsSignalTask;
}

/******************************************************************************
 SetPath

 ******************************************************************************/

void
JXFSRunCommandDialog::SetPath
	(
	const JCharacter* path
	)
{
	itsPathInput->SetText(path);
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXFSRunCommandDialog::Activate()
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
JXFSRunCommandDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 440,180, "");
	assert( window != NULL );

	JXFlatRect* ftcContainer =
		jnew JXFlatRect(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,120, 280,60);
	assert( ftcContainer != NULL );

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 290,20);
	assert( itsCmdInput != NULL );

	JXStaticText* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 290,20);
	assert( cmdLabel != NULL );
	cmdLabel->SetToLabel();

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 10,30, 60,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXFSRunCommandDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,30, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXFSRunCommandDialog::shortcuts::JXLayout"));

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,30, 60,20);
	assert( itsRunButton != NULL );
	itsRunButton->SetShortcuts(JGetString("itsRunButton::JXFSRunCommandDialog::shortcuts::JXLayout"));

	itsCmdHistoryMenu =
		jnew JXFSCommandHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,90, 30,20);
	assert( itsCmdHistoryMenu != NULL );

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,90, 60,20);
	assert( itsChooseCmdButton != NULL );

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 120,20);
	assert( itsUseShellCB != NULL );
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::JXFSRunCommandDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 120,20);
	assert( itsUseWindowCB != NULL );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::JXFSRunCommandDialog::shortcuts::JXLayout"));

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,0, 90,20);
	assert( itsStayOpenCB != NULL );

	JXStaticText* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 290,20);
	assert( directoryLabel != NULL );
	directoryLabel->SetToLabel();

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,40, 60,20);
	assert( itsChoosePathButton != NULL );

	itsPathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsPathInput != NULL );

	itsPathHistoryMenu =
		jnew JXPathHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);
	assert( itsPathHistoryMenu != NULL );

// end JXLayout

	window->SetTitle("Run command");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);

	ListenTo(itsRunButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsChoosePathButton);
	ListenTo(itsChooseCmdButton);
	ListenTo(itsPathHistoryMenu);
	ListenTo(itsCmdHistoryMenu);

	itsPathInput->ShouldAllowInvalidPath();
	itsPathInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsPathInput);

	const JFont& font = window->GetFontManager()->GetDefaultMonospaceFont();
	itsPathHistoryMenu->SetDefaultFont(font, kJTrue);

	itsCmdInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(font);
	itsCmdHistoryMenu->SetDefaultFont(font, kJTrue);

	itsStayOpenCB->SetState(kJTrue);

	if (!ReadSetup(kJFalse))
		{
		window->PlaceAsDialogWindow();
		}

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXFSRunCommandDialog::UpdateDisplay()
{
	JString p;
	if (!itsCmdInput->IsEmpty() && itsPathInput->GetPath(&p))
		{
		itsRunButton->Activate();
		}
	else
		{
		itsRunButton->Deactivate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSRunCommandDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRunButton && message.Is(JXButton::kPushed))
		{
		Exec();
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
		(JXGetHelpManager())->ShowSection(kHelpSectionName);
		}

	else if (sender == itsPathHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		itsPathHistoryMenu->UpdateInputField(message, itsPathInput);
		}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		itsPathInput->ChoosePath("", NULL);
		}

	else if (sender == itsCmdHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		JXFSRunFileDialog::HandleHistoryMenu(message, itsCmdHistoryMenu, itsCmdInput,
											 itsUseShellCB, itsUseWindowCB, NULL);
		}
	else if (sender == itsChooseCmdButton && message.Is(JXButton::kPushed))
		{
		JXFSRunFileDialog::HandleChooseCmdButton(itsCmdInput);
		}

	else if ((sender == itsCmdInput || sender == itsPathInput) &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateDisplay();
		}

	else if (sender == itsSignalTask && message.Is(JXCheckModTimeTask::kFileChanged))
		{
		ReadSetup(kJTrue);
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
JXFSRunCommandDialog::Exec()
{
	JString path;
	if (!itsPathInput->GetPath(&path))
		{
		return;
		}

	JXGetApplication()->DisplayBusyCursor();	// indicate to user that something happened

	const JFSBinding::CommandType type =
		JFSBinding::GetCommandType(itsUseShellCB->IsChecked(),
								   itsUseWindowCB->IsChecked());

	const JError err =
		(JXFSBindingManager::Instance())->Exec(path, itsCmdInput->GetText(), type);

	if (err.OK())
		{
		itsPathHistoryMenu->AddString(itsPathInput->GetText());
		itsCmdHistoryMenu->AddCommand(itsCmdInput->GetText(),
									  itsUseShellCB->IsChecked(),
									  itsUseWindowCB->IsChecked(),
									  kJFalse);
		WriteSetup();
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

JBoolean
JXFSRunCommandDialog::ReadSetup
	(
	const JBoolean update
	)
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
				if (update)
					{
					UpdateSetup(input);
					}
				else
					{
					ReadSetup(input);
					}
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
 WriteSetup

 ******************************************************************************/

void
JXFSRunCommandDialog::WriteSetup()
{
	JPrefsFile* file = NULL;
	if ((JPrefsFile::Create(kPrefsFileRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
		{
		std::ostringstream data;
		WriteSetup(data);
		file->SetData(kCurrentPrefsVersion, data);
		jdelete file;

		std::ofstream touch(itsSignalTask->GetFileName());
		touch.close();
		itsSignalTask->UpdateModTime();
		}
}

/******************************************************************************
 ReadSetup (private)

	ReadSetup() guarantees readable version

 ******************************************************************************/

void
JXFSRunCommandDialog::ReadSetup
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
	itsPathInput->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
	itsCmdInput->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> checked;
	itsUseShellCB->SetState(checked);

	input >> checked;
	itsUseWindowCB->SetState(checked);

	input >> checked;
	itsStayOpenCB->SetState(checked);
}

/******************************************************************************
 UpdateSetup (private)

	ReadSetup() guarantees readable version

 ******************************************************************************/

void
JXFSRunCommandDialog::UpdateSetup
	(
	std::istream& input
	)
{
	JXWindow::SkipGeometry(input);

	JString s;
	JBoolean checked;

	input >> s;
//	itsPathInput->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
//	itsCmdInput->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> checked;
//	itsUseShellCB->SetState(checked);

	input >> checked;
//	itsUseWindowCB->SetState(checked);

	input >> checked;
//	itsStayOpenCB->SetState(checked);
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
JXFSRunCommandDialog::WriteSetup
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsPathInput->GetText();

	output << ' ';
	itsPathHistoryMenu->WriteSetup(output);

	output << ' ' << itsCmdInput->GetText();

	output << ' ';
	itsCmdHistoryMenu->WriteSetup(output);

	output << ' ' << itsUseShellCB->IsChecked();
	output << ' ' << itsUseWindowCB->IsChecked();
	output << ' ' << itsStayOpenCB->IsChecked();
}
