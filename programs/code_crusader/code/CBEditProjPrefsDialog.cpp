/******************************************************************************
 CBEditProjPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditProjPrefsDialog.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
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

	JXWindow* window = jnew JXWindow(this, 360,310, JString::empty);
	assert( window != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,280, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,280, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBEditProjPrefsDialog::shortcuts::JXLayout"));

	itsReopenTextFilesCB =
		jnew JXTextCheckbox(JGetString("itsReopenTextFilesCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 300,20);
	assert( itsReopenTextFilesCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,280, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditProjPrefsDialog::shortcuts::JXLayout"));

	itsDropFileActionRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,150, 204,114);
	assert( itsDropFileActionRG != nullptr );

	JXTextRadioButton* absolutePathRB =
		jnew JXTextRadioButton(CBProjectTable::kAbsolutePath, JGetString("absolutePathRB::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 180,20);
	assert( absolutePathRB != nullptr );

	JXTextRadioButton* projectRelativeLabel =
		jnew JXTextRadioButton(CBProjectTable::kProjectRelative, JGetString("projectRelativeLabel::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 180,20);
	assert( projectRelativeLabel != nullptr );

	JXTextRadioButton* homeDirLabel =
		jnew JXTextRadioButton(CBProjectTable::kHomeRelative, JGetString("homeDirLabel::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 180,20);
	assert( homeDirLabel != nullptr );

	JXStaticText* dndPathLabel =
		jnew JXStaticText(JGetString("dndPathLabel::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 330,20);
	assert( dndPathLabel != nullptr );
	dndPathLabel->SetToLabel();

	JXTextRadioButton* dndAskLabel =
		jnew JXTextRadioButton(CBProjectTable::kAskPathType, JGetString("dndAskLabel::CBEditProjPrefsDialog::JXLayout"), itsDropFileActionRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 180,20);
	assert( dndAskLabel != nullptr );

	itsDoubleSpaceCB =
		jnew JXTextCheckbox(JGetString("itsDoubleSpaceCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 300,20);
	assert( itsDoubleSpaceCB != nullptr );

	itsRebuildMakefileDailyCB =
		jnew JXTextCheckbox(JGetString("itsRebuildMakefileDailyCB::CBEditProjPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 300,20);
	assert( itsRebuildMakefileDailyCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditProjPrefsDialog"));
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
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBProjectHelp-Prefs");
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
		(docList->GetElement(i))->
			SetProjectPrefs(reopenTextFiles, doubleSpaceCompile,
							rebuildMakefileDaily, dropFileAction);
		}
}
