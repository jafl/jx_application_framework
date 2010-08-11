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
	const JCharacter*	postCheckoutCmd,
	const JCharacter*	gitStatusCmd,
	const JBoolean		del
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(terminalCmd, manViewCmd, postCheckoutCmd, gitStatusCmd, del);
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
	const JCharacter*	postCheckoutCmd,
	const JCharacter*	gitStatusCmd,
	const JBoolean		del
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 450,300, "");
    assert( window != NULL );

    itsManInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 160,110, 275,20);
    assert( itsManInput != NULL );

    itsTerminalInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 160,130, 275,20);
    assert( itsTerminalInput != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 150,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 150,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,270, 60,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 285,270, 60,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,150, 300,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetFontSize(8);
    obj3_JXLayout->SetToLabel();

    itsDelCB =
        new JXTextCheckbox(JGetString("itsDelCB::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 210,20);
    assert( itsDelCB != NULL );

    itsAllowSpaceCB =
        new JXTextCheckbox(JGetString("itsAllowSpaceCB::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 190,20);
    assert( itsAllowSpaceCB != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 310,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(8);
    obj4_JXLayout->SetToLabel();

    itsPostCheckoutInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 220,180, 215,20);
    assert( itsPostCheckoutInput != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,180, 210,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,200, 215,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetFontSize(8);
    obj6_JXLayout->SetToLabel();

    itsGitStatusInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 220,220, 215,20);
    assert( itsGitStatusInput != NULL );

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,220, 210,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Edit preferences");
	SetButtons(okButton, cancelButton);

	itsManInput->SetText(manViewCmd);
	itsManInput->SetIsRequired();
	itsManInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsTerminalInput->SetText(terminalCmd);
	itsTerminalInput->SetIsRequired();
	itsTerminalInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsPostCheckoutInput->SetText(postCheckoutCmd);
	itsPostCheckoutInput->SetIsRequired();
	itsPostCheckoutInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsGitStatusInput->SetText(gitStatusCmd);
	itsGitStatusInput->SetIsRequired();
	itsGitStatusInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsDelCB->SetState(del);

	itsAllowSpaceCB->SetState(JXSaveFileInput::WillAllowSpace());
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
SyGEditPrefsDialog::GetPrefs
	(
	JString*	terminalCmd,
	JString*	manViewCmd,
	JString*	postCheckoutCmd,
	JString*	gitStatusCmd,
	JBoolean*	del
	)
	const
{
	*terminalCmd     = itsTerminalInput->GetText();
	*manViewCmd      = itsManInput->GetText();
	*postCheckoutCmd = itsPostCheckoutInput->GetText();
	*gitStatusCmd    = itsGitStatusInput->GetText();

	*del = itsDelCB->IsChecked();

	JXSaveFileInput::ShouldAllowSpace(itsAllowSpaceCB->IsChecked());
}
