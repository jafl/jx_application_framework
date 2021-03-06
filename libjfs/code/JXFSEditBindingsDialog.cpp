/******************************************************************************
 JXFSEditBindingsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "JXFSEditBindingsDialog.h"
#include "JXFSBindingTable.h"
#include "JFSBindingList.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <JXFontManager.h>
#include <jXGlobals.h>
#include <JPrefsFile.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kHelpSectionName = "JFSBindingEditorHelp";

const JCoordinate kHeaderHeight = 20;

// setup information

static const JCharacter* kPrefsFileRoot = "jx/jfs/edit_bindings_dialog";
const JFileVersion kCurrentPrefsVersion = 0;

// string ID's

static const JCharacter* kWarnOverwriteID = "WarnOverwrite::JXFSEditBindingsDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

static const JCharacter* kUserMsg = NULL;

JXFSEditBindingsDialog::JXFSEditBindingsDialog
	(
	JFSBindingList* list
	)
	:
	JXDialogDirector(JXGetApplication(), kJFalse),
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

JBoolean
JXFSEditBindingsDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}
	else if (!CheckData())
		{
		return kJFalse;
		}

	if (itsNeedsSaveFlag)
		{
		const JUserNotification::CloseAction action =
			JGetUserNotification()->OKToClose("Save bindings before closing?");
		if (action == JUserNotification::kDontClose)
			{
			return kJFalse;
			}
		else if (action == JUserNotification::kSaveData)
			{
			return Save(kJTrue);
			}
		}

	return kJTrue;
}

/******************************************************************************
 CheckData (private)

 ******************************************************************************/

JBoolean
JXFSEditBindingsDialog::CheckData()
{
	if (!JXWindowDirector::OKToDeactivate() ||
		!itsTable->EndEditing())
		{
		return kJFalse;
		}

	if (!(itsShellCmd->GetText()).Contains("$q") &&
		!(itsShellCmd->GetText()).Contains("$u"))
		{
		itsShellCmd->Focus();
		(JGetUserNotification())->ReportError("You must specify either $q or $u");
		return kJFalse;
		}
	else if (!(itsWindowCmd->GetText()).Contains("$q") &&
			 !(itsWindowCmd->GetText()).Contains("$u"))
		{
		itsWindowCmd->Focus();
		(JGetUserNotification())->ReportError("You must specify either $q or $u");
		return kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 580,430, "");
	assert( window != NULL );

	itsSaveButton =
		jnew JXTextButton(JGetString("itsSaveButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,360, 50,20);
	assert( itsSaveButton != NULL );
	itsSaveButton->SetShortcuts(JGetString("itsSaveButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,400, 50,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 490,230);
	assert( scrollbarSet != NULL );

	JXTextButton* addButton =
		jnew JXTextButton(JGetString("addButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,30, 70,20);
	assert( addButton != NULL );
	addButton->SetShortcuts(JGetString("addButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	JXTextButton* removeButton =
		jnew JXTextButton(JGetString("removeButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,90, 70,20);
	assert( removeButton != NULL );

	itsRevertButton =
		jnew JXTextButton(JGetString("itsRevertButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,320, 50,20);
	assert( itsRevertButton != NULL );
	itsRevertButton->SetShortcuts(JGetString("itsRevertButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 510,260, 50,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	JXStaticText* cmdHint1 =
		jnew JXStaticText(JGetString("cmdHint1::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 130,370, 360,20);
	assert( cmdHint1 != NULL );
	cmdHint1->SetFontSize(JGetDefaultFontSize()-2);
	cmdHint1->SetToLabel();

	itsDefCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 200,250, 290,20);
	assert( itsDefCmd != NULL );

	JXStaticText* shellLabel =
		jnew JXStaticText(JGetString("shellLabel::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,330, 120,20);
	assert( shellLabel != NULL );
	shellLabel->SetToLabel();

	itsUseDefaultCB =
		jnew JXTextCheckbox(JGetString("itsUseDefaultCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,250, 190,20);
	assert( itsUseDefaultCB != NULL );

	JXStaticText* cmdHint2 =
		jnew JXStaticText(JGetString("cmdHint2::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 140,270, 350,20);
	assert( cmdHint2 != NULL );
	cmdHint2->SetFontSize(JGetDefaultFontSize()-2);
	cmdHint2->SetToLabel();

	JXStaticText* windowLabel =
		jnew JXStaticText(JGetString("windowLabel::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,350, 120,20);
	assert( windowLabel != NULL );
	windowLabel->SetToLabel();

	itsShellCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 190,330, 300,20);
	assert( itsShellCmd != NULL );

	itsWindowCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 190,350, 300,20);
	assert( itsWindowCmd != NULL );

	itsDefShellCB =
		jnew JXTextCheckbox(JGetString("itsDefShellCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 140,290, 90,20);
	assert( itsDefShellCB != NULL );

	itsDefWindowCB =
		jnew JXTextCheckbox(JGetString("itsDefWindowCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 240,290, 110,20);
	assert( itsDefWindowCB != NULL );

	itsDefSingleCB =
		jnew JXTextCheckbox(JGetString("itsDefSingleCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 370,290, 120,20);
	assert( itsDefSingleCB != NULL );

	JXTextButton* duplicateButton =
		jnew JXTextButton(JGetString("duplicateButton::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 500,60, 70,20);
	assert( duplicateButton != NULL );
	duplicateButton->SetShortcuts(JGetString("duplicateButton::JXFSEditBindingsDialog::shortcuts::JXLayout"));

	itsAutoShellCB =
		jnew JXTextCheckbox(JGetString("itsAutoShellCB::JXFSEditBindingsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 140,400, 320,20);
	assert( itsAutoShellCB != NULL );

// end JXLayout

	window->SetTitle("Edit file bindings");
	window->LockCurrentMinSize();

	// table

	itsTable =
		jnew JXFSBindingTable(itsBindingList, addButton, removeButton, duplicateButton,
							 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 100,100);
	assert( itsTable != NULL );
	itsTable->FitToEnclosure();
	itsTable->Move(0, kHeaderHeight);
	itsTable->AdjustSize(0, -kHeaderHeight);

	JXColHeaderWidget* header =
		jnew JXColHeaderWidget(itsTable,
							  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, 100,kHeaderHeight);
	assert( header != NULL );
	header->FitToEnclosure(kJTrue, kJFalse);

	itsTable->SetColTitles(header);
	ListenTo(itsTable);

	// other information

	const JFont& font = window->GetFontManager()->GetDefaultMonospaceFont();

	itsDefCmd->SetFont(font);
	itsDefCmd->SetIsRequired();
	itsDefCmd->ShouldBroadcastAllTextChanged(kJTrue);

	itsShellCmd->SetFont(font);
	itsShellCmd->SetIsRequired();
	itsShellCmd->ShouldBroadcastAllTextChanged(kJTrue);

	itsWindowCmd->SetFont(font);
	itsWindowCmd->SetIsRequired();
	itsWindowCmd->ShouldBroadcastAllTextChanged(kJTrue);

	Revert(kJFalse);

	ListenTo(itsSaveButton);
	ListenTo(itsRevertButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);

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
	if (sender == itsSaveButton && message.Is(JXButton::kPushed))
		{
		Save(kJTrue);
		}
	else if (sender == itsRevertButton && message.Is(JXButton::kPushed))
		{
		Revert(kJTrue);
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		EndDialog(kJTrue);
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		JXGetHelpManager()->ShowSection(kHelpSectionName);
		}

	else if (sender == itsTable && message.Is(JXFSBindingTable::kDataChanged))
		{
		NeedsSave();
		}
	else if (message.Is(JTextEditor::kTextChanged) ||
			 message.Is(JXCheckbox::kPushed))
		{
		NeedsSave();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 NeedsSave (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::NeedsSave()
{
	itsNeedsSaveFlag = kJTrue;
	itsSaveButton->Activate();
	itsRevertButton->Activate();
}

/******************************************************************************
 DataReverted (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::DataReverted()
{
	itsNeedsSaveFlag = kJFalse;
	itsSaveButton->Deactivate();
	itsRevertButton->Deactivate();
}

/******************************************************************************
 Save (private)

 ******************************************************************************/

JBoolean
JXFSEditBindingsDialog::Save
	(
	const JBoolean askReplace
	)
{
	if (!CheckData() ||
		(askReplace && itsBindingList->NeedsRevert() &&
		 !(JGetUserNotification())->AskUserYes(JGetString(kWarnOverwriteID))))
		{
		return kJFalse;
		}

	itsBindingList->ShouldUseDefaultCommand(itsUseDefaultCB->IsChecked());

	itsBindingList->SetDefaultCommand(itsDefCmd->GetText(),
									  JFSBinding::GetCommandType(itsDefShellCB->IsChecked(),
																 itsDefWindowCB->IsChecked()),
									  itsDefSingleCB->IsChecked());

	itsBindingList->SetShellCommand(itsShellCmd->GetText());
	itsBindingList->SetWindowCommand(itsWindowCmd->GetText());
	itsBindingList->ShouldAutoUseShellCommand(itsAutoShellCB->IsChecked());

	const JError err = itsBindingList->Save();
	if (err.OK())
		{
		itsOrigBindingList->Revert();
		DataReverted();
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		return kJFalse;
		}
}

/******************************************************************************
 AddBinding

 ******************************************************************************/

void
JXFSEditBindingsDialog::AddBinding
	(
	const JCharacter*				suffix,
	const JCharacter*				cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	if (itsNeedsSaveFlag)
		{
		itsBindingList->SetCommand(suffix, cmd, type, singleFile);
		itsTable->SyncWithBindingList();
		}
	else
		{
		Revert(kJTrue);
		}
}

/******************************************************************************
 CheckIfNeedRevert

 ******************************************************************************/

void
JXFSEditBindingsDialog::CheckIfNeedRevert()
{
	if (itsBindingList->NeedsRevert() && !itsNeedsSaveFlag)
		{
		Revert(kJTrue);
		}
}

/******************************************************************************
 Revert (private)

 ******************************************************************************/

void
JXFSEditBindingsDialog::Revert
	(
	const JBoolean updateList
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
	JBoolean singleFile;
	itsDefCmd->SetText(b->GetCommand(&type, &singleFile));

	itsDefShellCB->SetState(JI2B( type == JFSBinding::kRunInShell ));
	itsDefWindowCB->SetState(JI2B( type == JFSBinding::kRunInWindow ));
	itsDefSingleCB->SetState(singleFile);

	itsShellCmd->SetText(itsBindingList->GetShellCommand());
	itsWindowCmd->SetText(itsBindingList->GetWindowCommand());

	itsAutoShellCB->SetState(itsBindingList->WillAutoUseShellCommand());

	DataReverted();
}

/******************************************************************************
 ReadSetup (private)

 ******************************************************************************/

JBoolean
JXFSEditBindingsDialog::ReadSetup()
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
JXFSEditBindingsDialog::WriteSetup()
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
{
	GetWindow()->WriteGeometry(output);
}
