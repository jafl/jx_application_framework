/******************************************************************************
 CBRunCommandDialog.cpp.cc

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBRunCommandDialog.h"
#include "CBProjectDocument.h"
#include "CBCommandManager.h"
#include "CBCommandPathInput.h"
#include "cbHelpText.h"
#include "cbGlobals.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXDocumentMenu.h>
#include <JXPathHistoryMenu.h>
#include <JXChooseSaveFile.h>
#include <JXFSRunFileDialog.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

// Save Command menu

static const JCharacter* kSaveCmdMenuStr =
	"  For all projects"
	"| For active project";

enum
{
	kSaveForAllCmd = 1,
	kSaveForActiveCmd
};

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version  1 saves isVCS flag

// string ID's

static const JCharacter* kWindowTitleID   = "WindowTitle::CBRunCommandDialog";
static const JCharacter* kSaveForActiveID = "SaveForActive::CBRunCommandDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBRunCommandDialog::CBRunCommandDialog
	(
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBRunCommandDialogID),
	itsProjDoc(projDoc),
	itsTextDoc(textDoc),
	itsFullNameList(NULL),
	itsLineIndexList(NULL)
{
	assert( itsTextDoc != NULL );

	BuildWindow();
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

CBRunCommandDialog::CBRunCommandDialog
	(
	CBProjectDocument*			projDoc,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue),
	JPrefObject(CBGetPrefsManager(), kCBRunCommandDialogID),
	itsProjDoc(projDoc),
	itsTextDoc(NULL)
{
	itsFullNameList = new JPtrArray<JString>(fullNameList, JPtrArrayT::kDeleteAll, kJTrue);
	assert( itsFullNameList != NULL );

	itsLineIndexList = new JArray<JIndex>(lineIndexList);
	assert( itsLineIndexList != NULL );

	BuildWindow();
	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRunCommandDialog::~CBRunCommandDialog()
{
	delete itsFullNameList;
	delete itsLineIndexList;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBRunCommandDialog::Activate()
{
	JXDialogDirector::Activate();

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
CBRunCommandDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 470,260, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 210,235, 60,20);
	assert( cancelButton != NULL );

	itsRunButton =
		new JXTextButton(JGetString("itsRunButton::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 389,234, 62,22);
	assert( itsRunButton != NULL );
	itsRunButton->SetShortcuts(JGetString("itsRunButton::CBRunCommandDialog::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,235, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBRunCommandDialog::shortcuts::JXLayout"));

	itsPathInput =
		new CBCommandPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 320,20);
	assert( itsPathInput != NULL );

	itsSaveAllCB =
		new JXTextCheckbox(JGetString("itsSaveAllCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 220,20);
	assert( itsSaveAllCB != NULL );
	itsSaveAllCB->SetShortcuts(JGetString("itsSaveAllCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsOneAtATimeCB =
		new JXTextCheckbox(JGetString("itsOneAtATimeCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 220,20);
	assert( itsOneAtATimeCB != NULL );
	itsOneAtATimeCB->SetShortcuts(JGetString("itsOneAtATimeCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		new JXTextCheckbox(JGetString("itsUseWindowCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,130, 220,20);
	assert( itsUseWindowCB != NULL );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsIsMakeCB =
		new JXTextCheckbox(JGetString("itsIsMakeCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 220,20);
	assert( itsIsMakeCB != NULL );
	itsIsMakeCB->SetShortcuts(JGetString("itsIsMakeCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,90, 370,20);
	assert( itsCmdInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 130,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 200,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsPathHistoryMenu =
		new JXPathHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 340,40, 30,20);
	assert( itsPathHistoryMenu != NULL );

	itsChoosePathButton =
		new JXTextButton(JGetString("itsChoosePathButton::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,40, 60,20);
	assert( itsChoosePathButton != NULL );

	itsChooseCmdButton =
		new JXTextButton(JGetString("itsChooseCmdButton::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,90, 60,20);
	assert( itsChooseCmdButton != NULL );

	itsRaiseCB =
		new JXTextCheckbox(JGetString("itsRaiseCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,150, 220,20);
	assert( itsRaiseCB != NULL );
	itsRaiseCB->SetShortcuts(JGetString("itsRaiseCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsBeepCB =
		new JXTextCheckbox(JGetString("itsBeepCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,170, 220,20);
	assert( itsBeepCB != NULL );
	itsBeepCB->SetShortcuts(JGetString("itsBeepCB::CBRunCommandDialog::shortcuts::JXLayout"));

	itsSaveCmdMenu =
		new JXTextMenu(JGetString("itsSaveCmdMenu::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,235, 90,20);
	assert( itsSaveCmdMenu != NULL );

	itsIsCVSCB =
		new JXTextCheckbox(JGetString("itsIsCVSCB::CBRunCommandDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 220,20);
	assert( itsIsCVSCB != NULL );
	itsIsCVSCB->SetShortcuts(JGetString("itsIsCVSCB::CBRunCommandDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString(kWindowTitleID));
	SetButtons(itsRunButton, cancelButton);
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);
	ListenTo(itsChoosePathButton);
	ListenTo(itsChooseCmdButton);
	ListenTo(itsPathHistoryMenu);
	ListenTo(itsIsMakeCB);
	ListenTo(itsIsCVSCB);
	ListenTo(itsUseWindowCB);

	itsPathInput->SetText("." ACE_DIRECTORY_SEPARATOR_STR);
	itsPathInput->ShouldAllowInvalidPath();
	itsPathInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsPathInput);

	if (itsProjDoc != NULL)
		{
		itsPathInput->SetBasePath(itsProjDoc->GetFilePath());
		}

	itsCmdInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsCmdInput);

	itsCmdInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsSaveCmdMenu->SetMenuItems(kSaveCmdMenuStr, "CBRunCommandDialog");
	ListenTo(itsSaveCmdMenu);

	// create hidden JXDocument so Meta-# shortcuts work

	JXDocumentMenu* fileListMenu =
		new JXDocumentMenu("", window,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,-20, 10,10);
	assert( fileListMenu != NULL );

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBRunCommandDialog::UpdateDisplay()
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

	if (itsIsMakeCB->IsChecked())
		{
		itsSaveAllCB->Deactivate();
		itsSaveAllCB->SetState(kJTrue);

		itsUseWindowCB->Deactivate();
		itsUseWindowCB->SetState(kJTrue);
		}
	else if (itsIsCVSCB->IsChecked())
		{
		itsSaveAllCB->Deactivate();
		itsSaveAllCB->SetState(kJTrue);

		itsUseWindowCB->Activate();
		}
	else
		{
		itsSaveAllCB->Activate();
		itsUseWindowCB->Activate();
		}

	if (itsUseWindowCB->IsChecked())
		{
		itsRaiseCB->Activate();
		}
	else
		{
		itsRaiseCB->Deactivate();
		itsRaiseCB->SetState(kJFalse);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBRunCommandDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			Exec();
			JPrefObject::WritePrefs();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBTasksHelpName);
		}

	else if (sender == itsSaveCmdMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateSaveCmdMenu();
		}
	else if (sender == itsSaveCmdMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleSaveCmdMenu(selection->GetIndex());
		}

	else if (sender == itsPathHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		itsPathHistoryMenu->UpdateInputField(message, itsPathInput);
		}
	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		itsPathInput->ChoosePath("", NULL);
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

	else if ((sender == itsIsMakeCB ||
			  sender == itsIsCVSCB  ||
			  sender == itsUseWindowCB) &&
			 message.Is(JXCheckbox::kPushed))
		{
		UpdateDisplay();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
CBRunCommandDialog::Exec()
{
	JString* path = new JString;
	assert( path != NULL );

	if (!itsPathInput->GetPath(path))
		{
		delete path;
		return;
		}
	itsPathHistoryMenu->AddString(itsPathInput->GetText());

	JString* cmd = new JString(itsCmdInput->GetText());
	assert( cmd != NULL );

	JString* ss = new JString;
	assert( ss != NULL );

	JString* mt = new JString;
	assert( mt != NULL );

	JString* ms = new JString;
	assert( ms != NULL );

	JString* mi = new JString;
	assert( mi != NULL );

	CBCommandManager::CmdInfo info(path, cmd, ss,
								   itsIsMakeCB->IsChecked(), itsIsCVSCB->IsChecked(),
								   itsSaveAllCB->IsChecked(), itsOneAtATimeCB->IsChecked(),
								   itsUseWindowCB->IsChecked(), itsRaiseCB->IsChecked(),
								   itsBeepCB->IsChecked(), mt, ms, mi, kJFalse);

	if (itsTextDoc != NULL)
		{
		CBCommandManager::Exec(info, itsProjDoc, itsTextDoc);
		}
	else if (itsFullNameList != NULL)
		{
		CBCommandManager::Exec(info, itsProjDoc, *itsFullNameList, *itsLineIndexList);
		}

	info.Free();
}

/******************************************************************************
 UpdateSaveCmdMenu (private)

 ******************************************************************************/

void
CBRunCommandDialog::UpdateSaveCmdMenu()
{
	itsSaveCmdMenu->EnableItem(kSaveForAllCmd);

	if (itsProjDoc != NULL)
		{
		itsSaveCmdMenu->EnableItem(kSaveForActiveCmd);

		const JCharacter* map[] =
			{
			"project", (itsProjDoc->GetName()).GetCString()
			};
		const JString s = JGetString(kSaveForActiveID, map, sizeof(map));
		itsSaveCmdMenu->SetItemText(kSaveForActiveCmd, s);
		}
}

/******************************************************************************
 HandleSaveCmdMenu (private)

 ******************************************************************************/

void
CBRunCommandDialog::HandleSaveCmdMenu
	(
	const JIndex index
	)
{
	if (index == kSaveForAllCmd)
		{
		AddCommandToMenu(CBGetCommandManager());
		(CBGetCommandManager())->UpdateMenuIDs();
		}
	else if (index == kSaveForActiveCmd)
		{
		if (itsProjDoc != NULL)
			{
			AddCommandToMenu(itsProjDoc->GetCommandManager());
			}
		}
}

/******************************************************************************
 AddCommandToMenu (private)

 ******************************************************************************/

void
CBRunCommandDialog::AddCommandToMenu
	(
	CBCommandManager* mgr
	)
{
	mgr->AppendCommand(itsPathInput->GetText(), itsCmdInput->GetText(), "",
					   itsIsMakeCB->IsChecked(), itsIsCVSCB->IsChecked(),
					   itsSaveAllCB->IsChecked(), itsOneAtATimeCB->IsChecked(),
					   itsUseWindowCB->IsChecked(), itsRaiseCB->IsChecked(),
					   itsBeepCB->IsChecked(), "", "", kJFalse);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBRunCommandDialog::ReadPrefs
	(
	istream& input
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

	JString s;
	JBoolean checked;

	input >> s;
	itsPathInput->SetText(s);

	itsPathHistoryMenu->ReadSetup(input);

	input >> s;
	itsCmdInput->SetText(s);

	input >> checked;
	itsIsMakeCB->SetState(checked);

	if (vers >= 1)
		{
		input >> checked;
		itsIsCVSCB->SetState(checked);
		}

	input >> checked;
	itsSaveAllCB->SetState(checked);

	input >> checked;
	itsOneAtATimeCB->SetState(checked);

	input >> checked;
	itsUseWindowCB->SetState(checked);

	input >> checked;
	itsRaiseCB->SetState(checked);

	input >> checked;
	itsBeepCB->SetState(checked);

	itsOneAtATimeCB->SetState(kJTrue);
	itsOneAtATimeCB->Deactivate();
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBRunCommandDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsPathInput->GetText();

	output << ' ';
	itsPathHistoryMenu->WriteSetup(output);

	output << ' ' << itsCmdInput->GetText();

	output << ' ' << itsIsMakeCB->IsChecked();
	output << ' ' << itsIsCVSCB->IsChecked();
	output << ' ' << itsSaveAllCB->IsChecked();
	output << ' ' << itsOneAtATimeCB->IsChecked();
	output << ' ' << itsUseWindowCB->IsChecked();
	output << ' ' << itsRaiseCB->IsChecked();
	output << ' ' << itsBeepCB->IsChecked();
}
