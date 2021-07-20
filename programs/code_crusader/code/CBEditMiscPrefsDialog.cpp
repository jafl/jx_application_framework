/******************************************************************************
 CBEditMiscPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

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
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditMiscPrefsDialog::CBEditMiscPrefsDialog()
	:
	JXDialogDirector(CBGetApplication(), true)
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

	auto* window = jnew JXWindow(this, 370,390, JString::empty);
	assert( window != nullptr );

	auto* warnTitle =
		jnew JXStaticText(JGetString("warnTitle::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,170, 80,20);
	assert( warnTitle != nullptr );
	warnTitle->SetToLabel();

	itsSaveAllCB =
		jnew JXTextCheckbox(JGetString("itsSaveAllCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,260, 120,20);
	assert( itsSaveAllCB != nullptr );

	itsCloseAllCB =
		jnew JXTextCheckbox(JGetString("itsCloseAllCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,290, 120,20);
	assert( itsCloseAllCB != nullptr );

	itsQuitCB =
		jnew JXTextCheckbox(JGetString("itsQuitCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,320, 120,20);
	assert( itsQuitCB != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,360, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,360, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBEditMiscPrefsDialog::shortcuts::JXLayout"));

	auto* noArgsTitle =
		jnew JXStaticText(JGetString("noArgsTitle::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,170, 180,20);
	assert( noArgsTitle != nullptr );
	noArgsTitle->SetToLabel();

	itsNewEditorCB =
		jnew JXTextCheckbox(JGetString("itsNewEditorCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,200, 150,20);
	assert( itsNewEditorCB != nullptr );

	itsNewProjectCB =
		jnew JXTextCheckbox(JGetString("itsNewProjectCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,230, 150,20);
	assert( itsNewProjectCB != nullptr );

	itsChooseFileCB =
		jnew JXTextCheckbox(JGetString("itsChooseFileCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,310, 150,20);
	assert( itsChooseFileCB != nullptr );

	itsReopenLastCB =
		jnew JXTextCheckbox(JGetString("itsReopenLastCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,260, 150,20);
	assert( itsReopenLastCB != nullptr );

	auto* firstTimeHint =
		jnew JXStaticText(JGetString("firstTimeHint::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,280, 100,20);
	assert( firstTimeHint != nullptr );
	firstTimeHint->SetToLabel();

	itsCopyWhenSelectCB =
		jnew JXTextCheckbox(JGetString("itsCopyWhenSelectCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 330,20);
	assert( itsCopyWhenSelectCB != nullptr );

	itsCloseUnsavedCB =
		jnew JXTextCheckbox(JGetString("itsCloseUnsavedCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,230, 120,20);
	assert( itsCloseUnsavedCB != nullptr );

	itsMacStyleCB =
		jnew JXTextCheckbox(JGetString("itsMacStyleCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 330,20);
	assert( itsMacStyleCB != nullptr );

	auto* restartHint =
		jnew JXStaticText(JGetString("restartHint::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 290,20);
	assert( restartHint != nullptr );
	restartHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	restartHint->SetToLabel();

	itsOpenOldProjCB =
		jnew JXTextCheckbox(JGetString("itsOpenOldProjCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,200, 120,20);
	assert( itsOpenOldProjCB != nullptr );

	itsFocusInDockCB =
		jnew JXTextCheckbox(JGetString("itsFocusInDockCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 330,20);
	assert( itsFocusInDockCB != nullptr );

	itsMiddleButtonPasteCB =
		jnew JXTextCheckbox(JGetString("itsMiddleButtonPasteCB::CBEditMiscPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 330,20);
	assert( itsMiddleButtonPasteCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditMiscPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle);
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
	itsMiddleButtonPasteCB->SetState(JXTEBase::MiddleButtonWillPaste());
	itsFocusInDockCB->SetState(JXWindow::WillFocusFollowCursorInDock());

	bool warnSave, warnClose, warnQuit;
	CBGetDocumentManager()->GetWarnings(&warnSave, &warnClose);
	CBGetApplication()->GetWarnings(&warnQuit);

	itsOpenOldProjCB->SetState(CBProjectDocument::WillAskOKToOpenOldVersion());
	itsCloseUnsavedCB->SetState(JXFileDocument::WillAskOKToClose());
	itsSaveAllCB->SetState(warnSave);
	itsCloseAllCB->SetState(warnClose);
	itsQuitCB->SetState(warnQuit);

	bool newEditor  = false, newProject = false,
			 reopenLast = true,  chooseFile = false;
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

	auto* group =
		jnew JXAtLeastOneCBGroup(4, itsNewEditorCB, itsNewProjectCB,
								itsReopenLastCB, itsChooseFileCB);
	assert( group != nullptr );
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
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
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

	CBGetDocumentManager()->SetWarnings(itsSaveAllCB->IsChecked(),
										  itsCloseAllCB->IsChecked());

	CBGetApplication()->SetWarnings(itsQuitCB->IsChecked());

	CBMDIServer* mdi;
	if (CBGetMDIServer(&mdi))
		{
		mdi->SetStartupOptions(itsNewEditorCB->IsChecked(),
							   itsNewProjectCB->IsChecked(),
							   itsReopenLastCB->IsChecked(),
							   itsChooseFileCB->IsChecked());
		}
}
