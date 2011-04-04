/******************************************************************************
 SyGEditPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGEditPrefsDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXSaveFileInput.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGEditPrefsDialog::SyGEditPrefsDialog
	(
	const JCharacter*	terminalCmd,
	const JCharacter*	manViewCmd,
	const JCharacter*	gitStatusCmd,
	const JCharacter*	gitHistoryCmd,
	const JCharacter*	postCheckoutCmd,
	const JBoolean		del,
	const JBoolean		newWindows,
	const JBoolean		perFolderPrefs
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(terminalCmd, manViewCmd, gitStatusCmd, gitHistoryCmd,
				postCheckoutCmd, del, newWindows, perFolderPrefs);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGEditPrefsDialog::~SyGEditPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SyGEditPrefsDialog::BuildWindow
	(
	const JCharacter*	terminalCmd,
	const JCharacter*	manViewCmd,
	const JCharacter*	gitStatusCmd,
	const JCharacter*	gitHistoryCmd,
	const JCharacter*	postCheckoutCmd,
	const JBoolean		del,
	const JBoolean		newWindows,
	const JBoolean		perFolderPrefs
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 470,380, "");
	assert( window != NULL );

	itsManInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 155,190, 300,20);
	assert( itsManInput != NULL );

	itsTerminalInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 155,210, 300,20);
	assert( itsTerminalInput != NULL );

	itsGitStatusInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,260, 240,20);
	assert( itsGitStatusInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,210, 145,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,190, 145,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,350, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 305,350, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 155,230, 300,20);
	assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetFontSize(8);
	obj3_JXLayout->SetToLabel();

	itsDelCB =
		new JXTextCheckbox(JGetString("itsDelCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 210,20);
	assert( itsDelCB != NULL );

	itsAllowSpaceCB =
		new JXTextCheckbox(JGetString("itsAllowSpaceCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,120, 210,20);
	assert( itsAllowSpaceCB != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,90, 310,20);
	assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(8);
	obj4_JXLayout->SetToLabel();

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,300, 205,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	JXStaticText* obj6_JXLayout =
		new JXStaticText(JGetString("obj6_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 215,320, 240,20);
	assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetFontSize(8);
	obj6_JXLayout->SetToLabel();

	JXStaticText* obj7_JXLayout =
		new JXStaticText(JGetString("obj7_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,260, 205,20);
	assert( obj7_JXLayout != NULL );
	obj7_JXLayout->SetToLabel();

	itsGitHistoryInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,280, 240,20);
	assert( itsGitHistoryInput != NULL );

	JXStaticText* obj8_JXLayout =
		new JXStaticText(JGetString("obj8_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,280, 205,20);
	assert( obj8_JXLayout != NULL );
	obj8_JXLayout->SetToLabel();

	itsPostCheckoutInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,300, 240,20);
	assert( itsPostCheckoutInput != NULL );

	itsOpenNewWindowsCB =
		new JXTextCheckbox(JGetString("itsOpenNewWindowsCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 210,20);
	assert( itsOpenNewWindowsCB != NULL );

	JXStaticText* obj9_JXLayout =
		new JXStaticText(JGetString("obj9_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 410,20);
	assert( obj9_JXLayout != NULL );
    obj9_JXLayout->SetFontSize(8);
	obj9_JXLayout->SetToLabel();

	itsFolderPrefsCB =
		new JXTextCheckbox(JGetString("itsFolderPrefsCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 220,20);
	assert( itsFolderPrefsCB != NULL );

// end JXLayout

	window->SetTitle("Edit preferences");
	SetButtons(okButton, cancelButton);

	itsManInput->SetText(manViewCmd);
	itsManInput->SetIsRequired();
	itsManInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsTerminalInput->SetText(terminalCmd);
	itsTerminalInput->SetIsRequired();
	itsTerminalInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsGitStatusInput->SetText(gitStatusCmd);
	itsGitStatusInput->SetIsRequired();
	itsGitStatusInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsGitHistoryInput->SetText(gitHistoryCmd);
	itsGitHistoryInput->SetIsRequired();
	itsGitHistoryInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsPostCheckoutInput->SetText(postCheckoutCmd);
	itsPostCheckoutInput->SetIsRequired();
	itsPostCheckoutInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsOpenNewWindowsCB->SetState(newWindows);
	itsDelCB->SetState(del);
	itsAllowSpaceCB->SetState(JXSaveFileInput::WillAllowSpace());
	itsFolderPrefsCB->SetState(perFolderPrefs);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
SyGEditPrefsDialog::GetPrefs
	(
	JString*	terminalCmd,
	JString*	manViewCmd,
	JString*	gitStatusCmd,
	JString*	gitHistoryCmd,
	JString*	postCheckoutCmd,
	JBoolean*	del,
	JBoolean*	newWindows,
	JBoolean*	perFolderPrefs
	)
	const
{
	*terminalCmd     = itsTerminalInput->GetText();
	*manViewCmd      = itsManInput->GetText();
	*gitStatusCmd    = itsGitStatusInput->GetText();
	*gitHistoryCmd   = itsGitHistoryInput->GetText();
	*postCheckoutCmd = itsPostCheckoutInput->GetText();

	*newWindows     = itsOpenNewWindowsCB->IsChecked();
	*del            = itsDelCB->IsChecked();
	*perFolderPrefs = itsFolderPrefsCB->IsChecked();

	JXSaveFileInput::ShouldAllowSpace(itsAllowSpaceCB->IsChecked());
}
