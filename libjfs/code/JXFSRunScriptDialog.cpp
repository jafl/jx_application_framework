/******************************************************************************
 JXFSRunScriptDialog.cpp.cc

	BASE CLASS = JXDialogDirector

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXFSRunScriptDialog.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXFontManager.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunScriptDialog::JXFSRunScriptDialog
	(
	const JCharacter* cmd
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSRunScriptDialog::~JXFSRunScriptDialog()
{
}

/******************************************************************************
 GetCommand

 ******************************************************************************/

const JString&
JXFSRunScriptDialog::GetCommand
	(
	JFSBinding::CommandType* type
	)
	const
{
	*type = JFSBinding::GetCommandType(itsUseShellCB->IsChecked(),
									   itsUseWindowCB->IsChecked());
	return itsCmdInput->GetText();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXFSRunScriptDialog::Activate()
{
	JXDialogDirector::Activate();

	if (IsActive())
		{
		itsCmdInput->SetCaretLocation(itsCmdInput->GetTextLength()+1);
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXFSRunScriptDialog::BuildWindow
	(
	const JCharacter* cmd
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 330,130, "");
	assert( window != NULL );

	itsCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsCmdInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,90, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* runButton =
		new JXTextButton(JGetString("runButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,90, 60,20);
	assert( runButton != NULL );
	runButton->SetShortcuts(JGetString("runButton::JXFSRunScriptDialog::shortcuts::JXLayout"));

	itsUseShellCB =
		new JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 120,20);
	assert( itsUseShellCB != NULL );
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::JXFSRunScriptDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		new JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 120,20);
	assert( itsUseWindowCB != NULL );
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::JXFSRunScriptDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Run script");
	SetButtons(runButton, cancelButton);

	JString s = cmd;
	s.AppendCharacter(' ');
	itsCmdInput->SetText(s);
	itsCmdInput->SetIsRequired();
	itsCmdInput->SetFont(window->GetFontManager()->GetDefaultMonospaceFont());
}
