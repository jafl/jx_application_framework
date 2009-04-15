/******************************************************************************
 JXEditWWWPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXEditWWWPrefsDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEditWWWPrefsDialog::JXEditWWWPrefsDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	showURLCmd,
	const JCharacter*	showFileContentCmd,
	const JCharacter*	showFileLocationCmd,
	const JCharacter*	composeMailCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(showURLCmd, showFileContentCmd, showFileLocationCmd, composeMailCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEditWWWPrefsDialog::~JXEditWWWPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXEditWWWPrefsDialog::BuildWindow
	(
	const JCharacter* showURLCmd,
	const JCharacter* showFileContentCmd,
	const JCharacter* showFileLocationCmd,
	const JCharacter* composeMailCmd
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 510,290, "");
    assert( window != NULL );
    SetWindow(window);

    itsShowURLCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 240,20, 250,20);
    assert( itsShowURLCmdInput != NULL );

    itsShowFileContentCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 240,80, 250,20);
    assert( itsShowFileContentCmdInput != NULL );

    itsShowFileLocationCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 240,140, 250,20);
    assert( itsShowFileLocationCmdInput != NULL );

    itsComposeMailCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 240,200, 250,20);
    assert( itsComposeMailCmdInput != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,260, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 319,259, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXEditWWWPrefsDialog::shortcuts::JXLayout"));

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,40, 150,20);
    assert( obj1_JXLayout != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,23, 220,20);
    assert( obj2_JXLayout != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,203, 220,20);
    assert( obj3_JXLayout != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,220, 170,20);
    assert( obj4_JXLayout != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,100, 140,20);
    assert( obj5_JXLayout != NULL );

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,83, 220,20);
    assert( obj6_JXLayout != NULL );

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,160, 250,20);
    assert( obj7_JXLayout != NULL );

    JXStaticText* obj8_JXLayout =
        new JXStaticText(JGetString("obj8_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,143, 220,20);
    assert( obj8_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Web Browser Preferences");
	SetButtons(okButton, cancelButton);

	itsShowURLCmdInput->SetText(showURLCmd);
	itsShowURLCmdInput->SetIsRequired();

	itsShowFileContentCmdInput->SetText(showFileContentCmd);
	itsShowFileContentCmdInput->SetIsRequired();

	itsShowFileLocationCmdInput->SetText(showFileLocationCmd);
	itsShowFileLocationCmdInput->SetIsRequired();

	itsComposeMailCmdInput->SetText(composeMailCmd);
	itsComposeMailCmdInput->SetIsRequired();
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
JXEditWWWPrefsDialog::GetPrefs
	(
	JString* showURLCmd,
	JString* showFileContentCmd,
	JString* showFileLocationCmd,
	JString* composeMailCmd
	)
	const
{
	*showURLCmd          = itsShowURLCmdInput->GetText();
	*showFileContentCmd  = itsShowFileContentCmdInput->GetText();
	*showFileLocationCmd = itsShowFileLocationCmdInput->GetText();
	*composeMailCmd      = itsComposeMailCmdInput->GetText();
}
