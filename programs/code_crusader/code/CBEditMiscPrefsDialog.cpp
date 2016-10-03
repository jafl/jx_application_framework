/******************************************************************************
 CBEditMiscPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditMiscPrefsDialog.h"
#include "CBProjectDocument.h"
#include "CBMDIServer.h"
#include "cbGlobals.h"
#include <JXFileDocument.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXTextCheckbox.h>
#include <JXAtLeastOneCBGroup.h>
#include <JXMenu.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditMiscPrefsDialog::CBEditMiscPrefsDialog()
	:
	JXDialogDirector(CBGetApplication(), kJTrue)
{
	BuildWindow();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditMiscPrefsDialog::~CBEditMiscPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditMiscPrefsDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,390, "");
	assert( window != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,170, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsSaveAllCB =
		new JXTextCheckbox(JGetString("itsSaveAllCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,260, 120,20);
	assert( itsSaveAllCB != NULL );

	itsCloseAllCB =
		new JXTextCheckbox(JGetString("itsCloseAllCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,290, 120,20);
	assert( itsCloseAllCB != NULL );

	itsQuitCB =
		new JXTextCheckbox(JGetString("itsQuitCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,320, 120,20);
	assert( itsQuitCB != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,360, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,360, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBEditMiscPrefsDialog::shortcuts::JXLayout"));

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 180,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsNewEditorCB =
		new JXTextCheckbox(JGetString("itsNewEditorCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,200, 150,20);
	assert( itsNewEditorCB != NULL );

	itsNewProjectCB =
		new JXTextCheckbox(JGetString("itsNewProjectCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,230, 150,20);
	assert( itsNewProjectCB != NULL );

	itsChooseFileCB =
		new JXTextCheckbox(JGetString("itsChooseFileCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,310, 150,20);
	assert( itsChooseFileCB != NULL );

	itsReopenLastCB =
		new JXTextCheckbox(JGetString("itsReopenLastCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 150,20);
	assert( itsReopenLastCB != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,280, 100,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	itsCopyWhenSelectCB =
		new JXTextCheckbox(JGetString("itsCopyWhenSelectCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 330,20);
	assert( itsCopyWhenSelectCB != NULL );

	itsCloseUnsavedCB =
		new JXTextCheckbox(JGetString("itsCloseUnsavedCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,230, 120,20);
	assert( itsCloseUnsavedCB != NULL );

	itsMacStyleCB =
		new JXTextCheckbox(JGetString("itsMacStyleCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 330,20);
	assert( itsMacStyleCB != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 290,20);
	assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(8);
	obj4_JXLayout->SetToLabel();

	itsOpenOldProjCB =
		new JXTextCheckbox(JGetString("itsOpenOldProjCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,200, 120,20);
	assert( itsOpenOldProjCB != NULL );

	itsFocusInDockCB =
		new JXTextCheckbox(JGetString("itsFocusInDockCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 330,20);
	assert( itsFocusInDockCB != NULL );

	itsMiddleButtonPasteCB =
		new JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 330,20);
	assert( itsMiddleButtonPasteCB != NULL );

// end JXLayout

	window->SetTitle("Miscellaneous Preferences");
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JI2B(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle));
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsMiddleButtonPasteCB->SetState(JXTEBase::MiddleButtonWillPaste());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());

	JBoolean warnSave, warnClose, warnQuit;
	(CBGetDocumentManager())->GetWarnings(&warnSave, &warnClose);
	(CBGetApplication())->GetWarnings(&warnQuit);

	itsOpenOldProjCB->SetState(CBProjectDocument::WillAskOKToOpenOldVersion());
	itsCloseUnsavedCB->SetState(JXFileDocument::WillAskOKToClose());
	itsSaveAllCB->SetState(warnSave);
	itsCloseAllCB->SetState(warnClose);
	itsQuitCB->SetState(warnQuit);

	JBoolean newEditor  = kJFalse, newProject = kJFalse,
			 reopenLast = kJTrue,  chooseFile = kJFalse;
	CBMDIServer* mdi;
	if (CBGetMDIServer(&mdi))
		{
		mdi->GetStartupOptions(&newEditor, &newProject,
							   &reopenLast, &chooseFile);
		}

	itsNewEditorCB->SetState(newEditor);
	itsNewProjectCB->SetState(newProject);
	itsReopenLastCB->SetState(reopenLast);
	itsChooseFileCB->SetState(chooseFile);

	// bind the checkboxes so at least one remains selected

	JXAtLeastOneCBGroup* group =
		new JXAtLeastOneCBGroup(4, itsNewEditorCB, itsNewProjectCB,
								itsReopenLastCB, itsChooseFileCB);
	assert( group != NULL );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditMiscPrefsDialog::Receive
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

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
CBEditMiscPrefsDialog::UpdateSettings()
{
	JXMenu::SetDisplayStyle(itsMacStyleCB->IsChecked() ?
							JXMenu::kMacintoshStyle : JXMenu::kWindowsStyle);
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
	JXTEBase::MiddleButtonShouldPaste(itsMiddleButtonPasteCB->IsChecked());
	JXWindow::ShouldFocusFollowCursorInDock(itsFocusInDockCB->IsChecked());
	JXFileDocument::ShouldAskOKToClose(itsCloseUnsavedCB->IsChecked());
	CBProjectDocument::ShouldAskOKToOpenOldVersion(itsOpenOldProjCB->IsChecked());

	(CBGetDocumentManager())->SetWarnings(itsSaveAllCB->IsChecked(),
										  itsCloseAllCB->IsChecked());

	(CBGetApplication())->SetWarnings(itsQuitCB->IsChecked());

	CBMDIServer* mdi;
	if (CBGetMDIServer(&mdi))
		{
		mdi->SetStartupOptions(itsNewEditorCB->IsChecked(),
							   itsNewProjectCB->IsChecked(),
							   itsReopenLastCB->IsChecked(),
							   itsChooseFileCB->IsChecked());
		}
}
