/******************************************************************************
 JXFSEditBindingsDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "JXFSEditBindingsDialog.h"
#include "JXFSBindingTable.h"
#include "JFSBindingList.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JPrefsFile.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

// setup information

static const JString kPrefsFileRoot("jx/jfs/edit_bindings_dialog");
const JFileVersion kCurrentPrefsVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

static JString kUserMsg;

JXFSEditBindingsDialog::JXFSEditBindingsDialog
	(
	JFSBindingList* list
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsBindingList(JFSBindingList::Create(&kUserMsg)),
	itsOrigBindingList(list)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSEditBindingsDialog::~JXFSEditBindingsDialog()
{
	WriteSetup();

	jdelete itsBindingList;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXFSEditBindingsDialog::OKToDeactivate()
{
	if (!JXWindowDirector::OKToDeactivate() || !CheckData())
	{
		return false;
	}

	if (itsSaveNeededFlag)
	{
		const JUserNotification::CloseAction action =
			JGetUserNotification()->OKToClose(JGetString("OKToClose::JXFSEditBindingsDialog"));
		if (action == JUserNotification::kDontClose)
		{
			return false;
		}
		else if (action == JUserNotification::kSaveData)
		{
			return Save(true);
		}
	}

	return true;
}

/******************************************************************************
 CheckData (private)

 ******************************************************************************/

bool
JXFSEditBindingsDialog::CheckData()
{
	if (!JXWindowDirector::OKToDeactivate() ||
		!itsTable->EndEditing())
	{
		return false;
	}

	if (!(itsShellCmd->GetText()->GetText()).Contains("$q") &&
		!(itsShellCmd->GetText()->GetText()).Contains("$u"))
	{
		itsShellCmd->Focus();
		JGetUserNotification()->ReportError(JGetString("MissingUorQ::JXFSEditBindingsDialog"));
		return false;
	}
	else if (!(itsWindowCmd->GetText()->GetText()).Contains("$q") &&
			 !(itsWindowCmd->GetText()->GetText()).Contains("$u"))
	{
		itsWindowCmd->Focus();
		JGetUserNotification()->ReportError(JGetString("MissingUorQ::JXFSEditBindingsDialog"));
		return false;
	}

	return true;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 580,430, JGetString("WindowTitle::JXFSEditBindingsDialog::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 490,260);
	assert( scrollbarSet != nullptr );

	auto* addButton =
		jnew JXTextButton(JGetString("addButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,30, 70,20);
	addButton->SetShortcuts(JGetString("addButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	auto* duplicateButton =
		jnew JXTextButton(JGetString("duplicateButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,60, 70,20);
	duplicateButton->SetShortcuts(JGetString("duplicateButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	auto* removeButton =
		jnew JXTextButton(JGetString("removeButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,90, 70,20);
	assert( removeButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,260, 50,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	itsUseDefaultCB =
		jnew JXTextCheckbox(JGetString("itsUseDefaultCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,280, 190,20);

	itsDefShellCB =
		jnew JXTextCheckbox(JGetString("itsDefShellCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 140,310, 90,20);

	itsDefWindowCB =
		jnew JXTextCheckbox(JGetString("itsDefWindowCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 240,310, 110,20);

	itsDefSingleCB =
		jnew JXTextCheckbox(JGetString("itsDefSingleCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 370,310, 120,20);

	itsRevertButton =
		jnew JXTextButton(JGetString("itsRevertButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,320, 50,20);
	itsRevertButton->SetShortcuts(JGetString("itsRevertButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	auto* shellCmdLabel =
		jnew JXStaticText(JGetString("shellCmdLabel::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,350, 120,20);
	shellCmdLabel->SetToLabel(false);

	itsSaveButton =
		jnew JXTextButton(JGetString("itsSaveButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,360, 50,20);
	itsSaveButton->SetShortcuts(JGetString("itsSaveButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	auto* windowCmdLabel =
		jnew JXStaticText(JGetString("windowCmdLabel::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,370, 120,20);
	windowCmdLabel->SetToLabel(false);

	itsAutoShellCB =
		jnew JXTextCheckbox(JGetString("itsAutoShellCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 140,400, 320,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,400, 50,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::JXFSEditBindingsDialog::JXLayout"));

	itsTable =
		jnew JXFSBindingTable(itsBindingList, addButton, removeButton, duplicateButton, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 490,240);

	auto* header =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 490,20);
	assert( header != nullptr );

	itsDefCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 200,280, 290,20);
	itsDefCmd->SetIsRequired();
	itsDefCmd->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsDefCmd->SetHint(JGetString("itsDefCmd::JXFSEditBindingsDialog::JXLayout"));

	itsShellCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 190,350, 300,20);
	itsShellCmd->SetIsRequired();
	itsShellCmd->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsShellCmd->SetHint(JGetString("itsShellCmd::JXFSEditBindingsDialog::JXLayout"));

	itsWindowCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 190,370, 300,20);
	itsWindowCmd->SetIsRequired();
	itsWindowCmd->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsWindowCmd->SetHint(JGetString("itsWindowCmd::JXFSEditBindingsDialog::JXLayout"));

// end JXLayout

	window->LockCurrentMinSize();

	itsTable->SetColTitles(header);
	ListenTo(itsTable, std::function([this](const JXFSBindingTable::DataChanged&)
	{
		SaveNeeded();
	}));

	Revert(false);

	ListenTo(itsSaveButton, std::function([this](const JXButton::Pushed&)
	{
		Save(true);
	}));

	ListenTo(itsRevertButton, std::function([this](const JXButton::Pushed&)
	{
		Revert(true);
	}));

	ListenTo(itsCloseButton, std::function([this](const JXButton::Pushed&)
	{
		if (Deactivate())
		{
			Close();
		}
	}));

	ListenTo(itsHelpButton, std::function([](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowSection(JGetString("HelpLink::JXFSEditBindingsDialog").GetBytes());
	}));

	ListenTo(itsUseDefaultCB);
	ListenTo(itsDefCmd);
	ListenTo(itsDefShellCB);
	ListenTo(itsDefWindowCB);
	ListenTo(itsDefSingleCB);
	ListenTo(itsShellCmd);
	ListenTo(itsWindowCmd);
	ListenTo(itsAutoShellCB);

	if (!ReadSetup())
	{
		window->PlaceAsDialogWindow();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSEditBindingsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JStyledText::kTextChanged) ||
		message.Is(JXCheckbox::kPushed))
	{
		SaveNeeded();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 SaveNeeded (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::SaveNeeded()
{
	itsSaveNeededFlag = true;
	itsSaveButton->Activate();
	itsRevertButton->Activate();
}

/******************************************************************************
 DataReverted (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::DataReverted()
{
	itsSaveNeededFlag = false;
	itsSaveButton->Deactivate();
	itsRevertButton->Deactivate();
}

/******************************************************************************
 Save (private)

 ******************************************************************************/

bool
JXFSEditBindingsDialog::Save
	(
	const bool askReplace
	)
{
	if (!CheckData() ||
		(askReplace && itsBindingList->NeedsRevert() &&
		 !JGetUserNotification()->AskUserYes(JGetString("WarnOverwrite::JXFSEditBindingsDialog"))))
	{
		return false;
	}

	itsBindingList->ShouldUseDefaultCommand(itsUseDefaultCB->IsChecked());

	itsBindingList->SetDefaultCommand(itsDefCmd->GetText()->GetText(),
									  JFSBinding::GetCommandType(itsDefShellCB->IsChecked(),
																 itsDefWindowCB->IsChecked()),
									  itsDefSingleCB->IsChecked());

	itsBindingList->SetShellCommand(itsShellCmd->GetText()->GetText());
	itsBindingList->SetWindowCommand(itsWindowCmd->GetText()->GetText());
	itsBindingList->ShouldAutoUseShellCommand(itsAutoShellCB->IsChecked());

	const JError err = itsBindingList->Save();
	if (err.OK())
	{
		itsOrigBindingList->Revert();
		DataReverted();
		return true;
	}
	else
	{
		err.ReportIfError();
		return false;
	}
}

/******************************************************************************
 AddBinding

 ******************************************************************************/

void
JXFSEditBindingsDialog::AddBinding
	(
	const JString&					suffix,
	const JString&					cmd,
	const JFSBinding::CommandType	type,
	const bool					singleFile
	)
{
	if (itsSaveNeededFlag)
	{
		itsBindingList->SetCommand(suffix, cmd, type, singleFile);
		itsTable->SyncWithBindingList();
	}
	else
	{
		Revert(true);
	}
}

/******************************************************************************
 CheckIfNeedRevert

 ******************************************************************************/

void
JXFSEditBindingsDialog::CheckIfNeedRevert()
{
	if (itsBindingList->NeedsRevert() && !itsSaveNeededFlag)
	{
		Revert(true);
	}
}

/******************************************************************************
 Revert (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::Revert
	(
	const bool updateList
	)
{
	if (updateList)
	{
		itsBindingList->Revert();
		itsTable->SyncWithBindingList();
	}

	itsUseDefaultCB->SetState(itsBindingList->WillUseDefaultCommand());

	const JFSBinding* b = itsBindingList->GetDefaultCommand();
	JFSBinding::CommandType type;
	bool singleFile;
	itsDefCmd->GetText()->SetText(b->GetCommand(&type, &singleFile));

	itsDefShellCB->SetState(type == JFSBinding::kRunInShell );
	itsDefWindowCB->SetState(type == JFSBinding::kRunInWindow );
	itsDefSingleCB->SetState(singleFile);

	itsShellCmd->GetText()->SetText(itsBindingList->GetShellCommand());
	itsWindowCmd->GetText()->SetText(itsBindingList->GetWindowCommand());

	itsAutoShellCB->SetState(itsBindingList->WillAutoUseShellCommand());

	DataReverted();
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

bool
JXFSEditBindingsDialog::ReadSetup()
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
JXFSEditBindingsDialog::WriteSetup()
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
JXFSEditBindingsDialog::ReadSetup
	(
	std::istream& input
	)
{
	JXWindow* w = GetWindow();
	w->ReadGeometry(input);
	w->Deiconify();
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::WriteSetup
	(
	std::ostream& output
	)
	const
{
	GetWindow()->WriteGeometry(output);
}
