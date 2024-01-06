/******************************************************************************
 JXFSRunFileDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXFSRunFileDialog.h"
#include "JXFSCommandHistoryMenu.h"
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JPrefsFile.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jTime.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

// setup information

static const JString kPrefsFileRoot("jx/jfs/run_file_dialog");
const JFileVersion kCurrentPrefsVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunFileDialog::JXFSRunFileDialog
	(
	const JString&	fileName,
	const bool	allowSaveCmd
	)
	:
	JXModalDialogDirector()
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

bool
JXFSRunFileDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		WriteSetup();	// here because we are invoked again before dtor
		return true;
	}
	else
	{
		itsCmdHistoryMenu->AddCommand(itsCmdInput->GetText()->GetText(),
									  itsUseShellCB->IsChecked(),
									  itsUseWindowCB->IsChecked(),
									  itsSingleFileCB->IsChecked());

		WriteSetup();	// here because we are invoked again before dtor
		return true;
	}
}

/******************************************************************************
 GetCommand

 ******************************************************************************/

const JString&
JXFSRunFileDialog::GetCommand
	(
	JFSBinding::CommandType*	type,
	bool*					singleFile,
	bool*					saveBinding
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
	const bool	allowSaveCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,170, JString::empty);

	auto* ftcContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 180,90, 250,40);

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 300,20);

	auto* prompt =
		jnew JXStaticText(JGetString("prompt::JXFSRunFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 300,20);
	assert( prompt != nullptr );
	prompt->SetToLabel();

	auto* cancelButton =
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

	auto* cmdHint =
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

	ListenTo(itsChooseCmdButton);
	ListenTo(itsHelpButton);
	ListenTo(itsCmdHistoryMenu);

	itsCmdInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsCmdHistoryMenu->SetDefaultFont(JFontManager::GetDefaultMonospaceFont(), true);

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

	ReadSetup();

	// adjust window width to fit prompt

	prompt->SetBreakCROnly(true);
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
		JXModalDialogDirector::Receive(sender, message);
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
	bool singleFile;
	const JString& cmd = menu->GetCommand(message, &type, &singleFile);

	cmdInput->Focus();
	cmdInput->GetText()->SetText(cmd);
	cmdInput->SelectAll();

	shellCB->SetState(false);
	windowCB->SetState(false);
	if (type == JFSBinding::kRunInShell)
	{
		shellCB->SetState(true);
	}
	else if (type == JFSBinding::kRunInWindow)
	{
		windowCB->SetState(true);
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
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile);
	if (dlog->DoDialog())
	{
		JString path, name;
		JSplitPathAndName(dlog->GetFullName(), &path, &name);
		name.Append(" ");
		cmdInput->GetText()->SetText(name);
		cmdInput->GoToEndOfLine();
	}
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

bool
JXFSRunFileDialog::ReadSetup()
{
	bool found = false;

	JPrefsFile* file = nullptr;
	if (JPrefsFile::Create(kPrefsFileRoot, &file,
						   JFileArray::kDeleteIfWaitTimeout).OK())
	{
		for (JFileVersion vers = kCurrentPrefsVersion; true; vers--)
		{
			if (file->IDValid(vers))
			{
				std::string data;
				file->GetData(vers, &data);
				std::istringstream input(data);
				ReadSetup(input);
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
 WriteSetup (private)

 ******************************************************************************/

void
JXFSRunFileDialog::WriteSetup()
	const
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
	bool checked;

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
