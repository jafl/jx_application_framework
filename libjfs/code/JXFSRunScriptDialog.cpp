/******************************************************************************
 JXFSRunScriptDialog.cpp.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by John Lindal. All rights reserved.

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

	JXWindow* window = jnew JXWindow(this, 330,130, "");
	assert( window != NULL );

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	assert( itsCmdInput != NULL );

	JXStaticText* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( cmdLabel != NULL );
	cmdLabel->SetToLabel();

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,90, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* runButton =
		jnew JXTextButton(JGetString("runButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,90, 60,20);
	assert( runButton != NULL );
	runButton->SetShortcuts(JGetString("runButton::JXFSRunScriptDialog::shortcuts::JXLayout"));

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 120,20);
	assert( itsUseShellCB != NULL );
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::JXFSRunScriptDialog::shortcuts::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunScriptDialog::JXLayout"), window,
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
