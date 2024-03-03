/******************************************************************************
 JXFSRunScriptDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXFSRunScriptDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSRunScriptDialog::JXFSRunScriptDialog
	(
	const JString& cmd
	)
	:
	JXModalDialogDirector()
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
	return itsCmdInput->GetText()->GetText();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXFSRunScriptDialog::Activate()
{
	JXModalDialogDirector::Activate();

	if (IsActive())
	{
		itsCmdInput->GoToEndOfLine();
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXFSRunScriptDialog::BuildWindow
	(
	const JString& cmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,130, JGetString("WindowTitle::JXFSRunScriptDialog::JXLayout"));

	auto* cmdLabel =
		jnew JXStaticText(JGetString("cmdLabel::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	cmdLabel->SetToLabel(false);

	itsUseShellCB =
		jnew JXTextCheckbox(JGetString("itsUseShellCB::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 120,20);
	itsUseShellCB->SetShortcuts(JGetString("itsUseShellCB::shortcuts::JXFSRunScriptDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,90, 60,20);
	assert( cancelButton != nullptr );

	auto* runButton =
		jnew JXTextButton(JGetString("runButton::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 250,90, 60,20);
	runButton->SetShortcuts(JGetString("runButton::shortcuts::JXFSRunScriptDialog::JXLayout"));

	itsUseWindowCB =
		jnew JXTextCheckbox(JGetString("itsUseWindowCB::JXFSRunScriptDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 120,20);
	itsUseWindowCB->SetShortcuts(JGetString("itsUseWindowCB::shortcuts::JXFSRunScriptDialog::JXLayout"));

	itsCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 290,20);
	itsCmdInput->SetIsRequired();
	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());

// end JXLayout

	SetButtons(runButton, cancelButton);

	JString s = cmd;
	s.Append(" ");
	itsCmdInput->GetText()->SetText(s);
	itsCmdInput->SetIsRequired();
	itsCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
}
