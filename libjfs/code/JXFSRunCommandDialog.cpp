/******************************************************************************
 JXFSRunCommandDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXFSRunCommandDialog.h"
#include "JXFSCommandHistoryMenu.h"
#include "JXFSBindingManager.h"
#include "JXFSRunFileDialog.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXCSFDialogBase.h>
#include <jx-af/jx/JXCheckModTimeTask.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JPrefsFile.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

static const JString kPrefsFileRoot("jx/jfs/run_cmd_dialog");
const JFileVersion kCurrentPrefsVersion = 0;

static const JString kSignalFileName("~/.jx/jfs/run_cmd_dialog.signal");
const Time kUpdateInterval = 1000;	// milliseconds

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
	const JString& path
	)
{
	itsPathInput->GetText()->SetText(path);
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

	auto* window = jnew JXWindow(this, 440,180, JGetString("WindowTitle::JXFSRunCommandDialog::JXLayout"));

	auto* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 290,20);
	directoryLabel->SetToLabel(false);

	itsPathHistoryMenu =
		jnew JXPathHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,40, 30,20);

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,40, 60,20);

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 290,20);
	cmdLabel->SetToLabel(false);

	itsCmdHistoryMenu =
		jnew JXFSCommandHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 310,90, 30,20);

	itsChooseCmdButton =
		jnew JXTextButton(JGetString("itsChooseCmdButton::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,90, 60,20);

	auto* ftcContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,120, 280,60);
	assert( ftcContainer != nullptr );

	itsStayOpenCB =
		jnew JXTextCheckbox(JGetString("itsStayOpenCB::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,0, 90,20);

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 120,20);
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::shortcuts::JXFSRunCommandDialog::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 120,20);
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::shortcuts::JXFSRunCommandDialog::JXLayout"));

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 10,30, 60,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::JXFSRunCommandDialog::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,30, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::JXFSRunCommandDialog::JXLayout"));

	itsRunButton =
		jnew JXTextButton(JGetString("itsRunButton::JXFSRunCommandDialog::JXLayout"), ftcContainer,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,30, 60,20);
	itsRunButton->SetShortcuts(JGetString("itsRunButton::shortcuts::JXFSRunCommandDialog::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 290,20);

	itsPathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	itsPathInput->SetIsRequired(true);
	itsPathInput->ShouldAllowInvalidPath(true);
	itsPathInput->ShouldRequireWritable(false);

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(true);

	ListenTo(itsRunButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);
	ListenTo(itsChoosePathButton);
	ListenTo(itsChooseCmdButton);
	ListenTo(itsPathHistoryMenu);
	ListenTo(itsCmdHistoryMenu);

	ListenTo(itsPathInput);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();
	itsPathHistoryMenu->SetDefaultFont(font, true);

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(font);
	itsCmdHistoryMenu->SetDefaultFont(font, true);

	itsStayOpenCB->SetState(true);

	if (!ReadSetup(false))
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
	if (!itsCmdInput->GetText()->IsEmpty() && itsPathInput->GetPath(&p))
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
		JXGetHelpManager()->ShowSection(JGetString("HelpLink::JXFSRunCommandDialog").GetBytes());
	}

	else if (sender == itsPathHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		itsPathHistoryMenu->UpdateInputField(message, itsPathInput);
	}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
	{
		itsPathInput->ChoosePath(JString::empty);
	}

	else if (sender == itsCmdHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		JXFSRunFileDialog::HandleHistoryMenu(message, itsCmdHistoryMenu, itsCmdInput,
											 itsUseShellCB, itsUseWindowCB, nullptr);
	}
	else if (sender == itsChooseCmdButton && message.Is(JXButton::kPushed))
	{
		JXFSRunFileDialog::HandleChooseCmdButton(itsCmdInput);
	}

	else if ((sender == itsCmdInput || sender == itsPathInput) &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else if (sender == itsSignalTask && message.Is(JXCheckModTimeTask::kFileChanged))
	{
		ReadSetup(true);
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
		JXFSBindingManager::Exec(path, itsCmdInput->GetText()->GetText(), type);

	if (err.OK())
	{
		itsPathHistoryMenu->AddString(itsPathInput->GetText()->GetText());
		itsCmdHistoryMenu->AddCommand(itsCmdInput->GetText()->GetText(),
									  itsUseShellCB->IsChecked(),
									  itsUseWindowCB->IsChecked(),
									  false);
		WriteSetup();
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

bool
JXFSRunCommandDialog::ReadSetup
	(
	const bool update
	)
{
	bool found = false;

	JPrefsFile* file = nullptr;
	if ((JPrefsFile::Create(kPrefsFileRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
	{
		for (JFileVersion vers = kCurrentPrefsVersion; true; vers--)
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
				found = true;
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
	JPrefsFile* file = nullptr;
	if ((JPrefsFile::Create(kPrefsFileRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
	{
		std::ostringstream data;
		WriteSetup(data);
		file->SetData(kCurrentPrefsVersion, data);
		jdelete file;

		std::ofstream touch(itsSignalTask->GetFileName().GetBytes());
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
	bool checked;

	input >> s;
	itsPathInput->GetText()->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
	itsCmdInput->GetText()->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> JBoolFromString(checked);
	itsUseShellCB->SetState(checked);

	input >> JBoolFromString(checked);
	itsUseWindowCB->SetState(checked);

	input >> JBoolFromString(checked);
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
	bool checked;

	input >> s;
//	itsPathInput->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
//	itsCmdInput->SetText(s);

	itsCmdHistoryMenu->ReadSetup(input);

	input >> JBoolFromString(checked);
//	itsUseShellCB->SetState(checked);

	input >> JBoolFromString(checked);
//	itsUseWindowCB->SetState(checked);

	input >> JBoolFromString(checked);
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

	output << ' ' << itsPathInput->GetText()->GetText();

	output << ' ';
	itsPathHistoryMenu->WriteSetup(output);

	output << ' ' << itsCmdInput->GetText()->GetText();

	output << ' ';
	itsCmdHistoryMenu->WriteSetup(output);

	output << ' ' << JBoolToString(itsUseShellCB->IsChecked())
				  << JBoolToString(itsUseWindowCB->IsChecked())
				  << JBoolToString(itsStayOpenCB->IsChecked());
}
