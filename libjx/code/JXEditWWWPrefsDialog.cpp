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

    JXWindow* window = new JXWindow(this, 520,290, "");
    assert( window != NULL );

    itsShowURLCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 250,20, 250,20);
    assert( itsShowURLCmdInput != NULL );

    itsShowFileContentCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 250,80, 250,20);
    assert( itsShowFileContentCmdInput != NULL );

    itsShowFileLocationCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 250,140, 250,20);
    assert( itsShowFileLocationCmdInput != NULL );

    itsComposeMailCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 250,200, 250,20);
    assert( itsComposeMailCmdInput != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,260, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 330,260, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXEditWWWPrefsDialog::shortcuts::JXLayout"));

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,40, 252,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 230,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetToLabel();

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,220, 250,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetToLabel();

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,100, 251,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetToLabel();

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,160, 260,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

    JXStaticText* obj8_JXLayout =
        new JXStaticText(JGetString("obj8_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 230,20);
    assert( obj8_JXLayout != NULL );
    obj8_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Web Browser Preferences");
	SetButtons(okButton, cancelButton);

	itsShowURLCmdInput->SetText(showURLCmd);
	itsShowURLCmdInput->SetIsRequired();
	itsShowURLCmdInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsShowFileContentCmdInput->SetText(showFileContentCmd);
	itsShowFileContentCmdInput->SetIsRequired();
	itsShowFileContentCmdInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsShowFileLocationCmdInput->SetText(showFileLocationCmd);
	itsShowFileLocationCmdInput->SetIsRequired();
	itsShowFileLocationCmdInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsComposeMailCmdInput->SetText(composeMailCmd);
	itsComposeMailCmdInput->SetIsRequired();
	itsComposeMailCmdInput->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());
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
