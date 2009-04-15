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
	const JBoolean		del
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(terminalCmd, manViewCmd, del);
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
	const JBoolean		del
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 450,220, "");
    assert( window != NULL );
    SetWindow(window);

    itsManInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 190,110, 245,20);
    assert( itsManInput != NULL );

    itsTerminalInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 190,130, 245,20);
    assert( itsTerminalInput != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 180,20);
    assert( obj1_JXLayout != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 180,20);
    assert( obj2_JXLayout != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,190, 60,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 284,189, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::SyGEditPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 150,150, 290,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetFontSize(10);

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
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 290,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(10);

// end JXLayout

	window->SetTitle("Edit preferences");
	SetButtons(okButton, cancelButton);

	itsManInput->SetText(manViewCmd);
	itsManInput->SetIsRequired();

	itsTerminalInput->SetText(terminalCmd);
	itsTerminalInput->SetIsRequired();

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
	JBoolean*	del
	)
	const
{
	*terminalCmd = itsTerminalInput->GetText();
	*manViewCmd  = itsManInput->GetText();

	*del = itsDelCB->IsChecked();

	JXSaveFileInput::ShouldAllowSpace(itsAllowSpaceCB->IsChecked());
}
