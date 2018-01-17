/******************************************************************************
 CBExtEditorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBExtEditorDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExtEditorDialog::CBExtEditorDialog
	(
	JXDirector*			supervisor,
	const JBoolean		editTextLocally,
	const JCharacter*	editTextFileCmd,
	const JCharacter*	editTextFileLineCmd,
	const JBoolean		editBinaryLocally,
	const JCharacter*	editBinaryFileCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(editTextLocally, editTextFileCmd, editTextFileLineCmd,
				editBinaryLocally, editBinaryFileCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBExtEditorDialog::~CBExtEditorDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBExtEditorDialog::BuildWindow
	(
	const JBoolean		editTextLocally,
	const JCharacter*	editTextFileCmd,
	const JCharacter*	editTextFileLineCmd,
	const JBoolean		editBinaryLocally,
	const JCharacter*	editBinaryFileCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 450,220, "");
	assert( window != NULL );

	itsEditTextFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 230,20);
	assert( itsEditTextFileCmdInput != NULL );

	itsEditTextFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,60, 230,20);
	assert( itsEditTextFileLineCmdInput != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,190, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,190, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBExtEditorDialog::shortcuts::JXLayout"));

	JXStaticText* editTextHint =
		jnew JXStaticText(JGetString("editTextHint::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,80, 270,20);
	assert( editTextHint != NULL );
	editTextHint->SetFontSize(JGetDefaultFontSize()-2);
	editTextHint->SetToLabel();

	JXStaticText* editTextLabel =
		jnew JXStaticText(JGetString("editTextLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,40, 170,20);
	assert( editTextLabel != NULL );
	editTextLabel->SetToLabel();

	JXStaticText* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,60, 170,20);
	assert( editLineLabel != NULL );
	editLineLabel->SetToLabel();

	itsExtTextEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtTextEditorCB::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 160,20);
	assert( itsExtTextEditorCB != NULL );

	itsEditBinaryFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,130, 230,20);
	assert( itsEditBinaryFileCmdInput != NULL );

	JXStaticText* editBinaryHint =
		jnew JXStaticText(JGetString("editBinaryHint::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,150, 230,20);
	assert( editBinaryHint != NULL );
	editBinaryHint->SetFontSize(JGetDefaultFontSize()-2);
	editBinaryHint->SetToLabel();

	JXStaticText* editBinaryLabel =
		jnew JXStaticText(JGetString("editBinaryLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,130, 170,20);
	assert( editBinaryLabel != NULL );
	editBinaryLabel->SetToLabel();

	itsExtBinaryEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtBinaryEditorCB::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 180,20);
	assert( itsExtBinaryEditorCB != NULL );

// end JXLayout

	window->SetTitle("External Editor");
	SetButtons(okButton, cancelButton);

	itsExtTextEditorCB->SetState(!editTextLocally);
	itsEditTextFileCmdInput->SetText(editTextFileCmd);
	itsEditTextFileCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsEditTextFileLineCmdInput->SetText(editTextFileLineCmd);
	itsEditTextFileLineCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsExtBinaryEditorCB->SetState(!editBinaryLocally);
	itsEditBinaryFileCmdInput->SetText(editBinaryFileCmd);
	itsEditBinaryFileCmdInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	// until we have a built-in one
	itsExtBinaryEditorCB->SetState(kJTrue);
	itsExtBinaryEditorCB->Deactivate();

	UpdateDisplay();
	ListenTo(itsExtTextEditorCB);
	ListenTo(itsExtBinaryEditorCB);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
CBExtEditorDialog::GetPrefs
	(
	JBoolean*	editTextLocally,
	JString*	editTextFileCmd,
	JString*	editTextFileLineCmd,
	JBoolean*	editBinaryLocally,
	JString*	editBinaryFileCmd
	)
	const
{
	*editTextLocally     = !itsExtTextEditorCB->IsChecked();
	*editTextFileCmd     = itsEditTextFileCmdInput->GetText();
	*editTextFileLineCmd = itsEditTextFileLineCmdInput->GetText();

	*editBinaryLocally   = !itsExtBinaryEditorCB->IsChecked();
	*editBinaryFileCmd   = itsEditBinaryFileCmdInput->GetText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBExtEditorDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == itsExtTextEditorCB || sender == itsExtBinaryEditorCB) &&
		message.Is(JXCheckbox::kPushed))
		{
		UpdateDisplay();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBExtEditorDialog::UpdateDisplay()
{
	if (itsExtTextEditorCB->IsChecked())
		{
		itsEditTextFileCmdInput->Activate();
		itsEditTextFileCmdInput->SetIsRequired(kJTrue);

		itsEditTextFileLineCmdInput->Activate();
		itsEditTextFileLineCmdInput->SetIsRequired(kJTrue);
		}
	else
		{
		itsEditTextFileCmdInput->Deactivate();
		itsEditTextFileCmdInput->SetIsRequired(kJFalse);

		itsEditTextFileLineCmdInput->Deactivate();
		itsEditTextFileLineCmdInput->SetIsRequired(kJFalse);
		}

	if (itsExtBinaryEditorCB->IsChecked())
		{
		itsEditBinaryFileCmdInput->Activate();
		itsEditBinaryFileCmdInput->SetIsRequired(kJTrue);
		}
	else
		{
		itsEditBinaryFileCmdInput->Deactivate();
		itsEditBinaryFileCmdInput->SetIsRequired(kJFalse);
		}
}
