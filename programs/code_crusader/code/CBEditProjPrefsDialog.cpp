/******************************************************************************
 CBEditProjPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditProjPrefsDialog.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditProjPrefsDialog::CBEditProjPrefsDialog
	(
	const JBoolean							reopenTextFiles,
	const JBoolean							doubleSpaceCompile,
	const JBoolean							rebuildMakefileDaily,
	const CBProjectTable::DropFileAction	dropFileAction
	)
	:
	JXDialogDirector(CBGetApplication(), kJTrue)
{
	BuildWindow(reopenTextFiles, doubleSpaceCompile, rebuildMakefileDaily,
				dropFileAction);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditProjPrefsDialog::~CBEditProjPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditProjPrefsDialog::BuildWindow
	(
	const JBoolean							reopenTextFiles,
	const JBoolean							doubleSpaceCompile,
	const JBoolean							rebuildMakefileDaily,
	const CBProjectTable::DropFileAction	dropFileAction
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 360,310, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,280, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,280, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBEditProjPrefsDialog::shortcuts::JXLayout"));

	itsReopenTextFilesCB =
		new JXTextCheckbox(JGetString("itsReopenTextFilesCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 300,20);
	assert( itsReopenTextFilesCB != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,280, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditProjPrefsDialog::shortcuts::JXLayout"));

	itsDropFileActionRG =
		new JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,150, 204,114);
	assert( itsDropFileActionRG != NULL );

	JXTextRadioButton* obj1_JXLayout =
		new JXTextRadioButton(CBProjectTable::kAbsolutePath, JGetString("obj1_JXLayout::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( obj1_JXLayout != NULL );

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(CBProjectTable::kProjectRelative, JGetString("obj2_JXLayout::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( obj2_JXLayout != NULL );

	JXTextRadioButton* obj3_JXLayout =
		new JXTextRadioButton(CBProjectTable::kHomeRelative, JGetString("obj3_JXLayout::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( obj3_JXLayout != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 330,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXTextRadioButton* obj5_JXLayout =
		new JXTextRadioButton(CBProjectTable::kAskPathType, JGetString("obj5_JXLayout::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 180,20);
	assert( obj5_JXLayout != NULL );

	itsDoubleSpaceCB =
		new JXTextCheckbox(JGetString("itsDoubleSpaceCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 300,20);
	assert( itsDoubleSpaceCB != NULL );

	itsRebuildMakefileDailyCB =
		new JXTextCheckbox(JGetString("itsRebuildMakefileDailyCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 300,20);
	assert( itsRebuildMakefileDailyCB != NULL );

// end JXLayout

	window->SetTitle("Project Preferences");
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	itsReopenTextFilesCB->SetState(reopenTextFiles);
	itsDoubleSpaceCB->SetState(doubleSpaceCompile);
	itsRebuildMakefileDailyCB->SetState(rebuildMakefileDaily);
	itsDropFileActionRG->SelectItem(dropFileAction);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditProjPrefsDialog::Receive
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
			UpdateSettings();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBProjectPrefsHelpName);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
CBEditProjPrefsDialog::UpdateSettings()
{
	(CBGetApplication())->DisplayBusyCursor();

	const JBoolean reopenTextFiles      = itsReopenTextFilesCB->IsChecked();
	const JBoolean doubleSpaceCompile   = itsDoubleSpaceCB->IsChecked();
	const JBoolean rebuildMakefileDaily = itsRebuildMakefileDailyCB->IsChecked();

	const CBProjectTable::DropFileAction dropFileAction =
		(CBProjectTable::DropFileAction) itsDropFileActionRG->GetSelectedItem();

	CBDocumentManager* docMgr             = CBGetDocumentManager();
	JPtrArray<CBProjectDocument>* docList = docMgr->GetProjectDocList();
	const JSize docCount = docList->GetElementCount();

	for (JIndex i=1; i<=docCount; i++)
		{
		(docList->NthElement(i))->
			SetProjectPrefs(reopenTextFiles, doubleSpaceCompile,
							rebuildMakefileDaily, dropFileAction);
		}
}
