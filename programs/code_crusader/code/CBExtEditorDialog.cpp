/******************************************************************************
 CBExtEditorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBExtEditorDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBExtEditorDialog::CBExtEditorDialog
	(
	JXDirector*		supervisor,
	const JBoolean	editTextLocally,
	const JString&	editTextFileCmd,
	const JString&	editTextFileLineCmd,
	const JBoolean	editBinaryLocally,
	const JString&	editBinaryFileCmd
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
	const JBoolean	editTextLocally,
	const JString&	editTextFileCmd,
	const JString&	editTextFileLineCmd,
	const JBoolean	editBinaryLocally,
	const JString&	editBinaryFileCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 450,220, JString::empty);
	assert( window != nullptr );

	itsEditTextFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 230,20);
	assert( itsEditTextFileCmdInput != nullptr );

	itsEditTextFileLineCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,60, 230,20);
	assert( itsEditTextFileLineCmdInput != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,190, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,190, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBExtEditorDialog::shortcuts::JXLayout"));

	JXStaticText* editTextHint =
		jnew JXStaticText(JGetString("editTextHint::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,80, 270,20);
	assert( editTextHint != nullptr );
	editTextHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	editTextHint->SetToLabel();

	JXStaticText* editTextLabel =
		jnew JXStaticText(JGetString("editTextLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,40, 170,20);
	assert( editTextLabel != nullptr );
	editTextLabel->SetToLabel();

	JXStaticText* editLineLabel =
		jnew JXStaticText(JGetString("editLineLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,60, 170,20);
	assert( editLineLabel != nullptr );
	editLineLabel->SetToLabel();

	itsExtTextEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtTextEditorCB::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 160,20);
	assert( itsExtTextEditorCB != nullptr );

	itsEditBinaryFileCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,130, 230,20);
	assert( itsEditBinaryFileCmdInput != nullptr );

	JXStaticText* editBinaryHint =
		jnew JXStaticText(JGetString("editBinaryHint::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,150, 230,20);
	assert( editBinaryHint != nullptr );
	editBinaryHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	editBinaryHint->SetToLabel();

	JXStaticText* editBinaryLabel =
		jnew JXStaticText(JGetString("editBinaryLabel::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,130, 170,20);
	assert( editBinaryLabel != nullptr );
	editBinaryLabel->SetToLabel();

	itsExtBinaryEditorCB =
		jnew JXTextCheckbox(JGetString("itsExtBinaryEditorCB::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 180,20);
	assert( itsExtBinaryEditorCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBExtEditorDialog"));
	SetButtons(okButton, cancelButton);

	itsExtTextEditorCB->SetState(!editTextLocally);
	itsEditTextFileCmdInput->GetText()->SetText(editTextFileCmd);
	itsEditTextFileCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsEditTextFileLineCmdInput->GetText()->SetText(editTextFileLineCmd);
	itsEditTextFileLineCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsExtBinaryEditorCB->SetState(!editBinaryLocally);
	itsEditBinaryFileCmdInput->GetText()->SetText(editBinaryFileCmd);
	itsEditBinaryFileCmdInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

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
	*editTextFileCmd     = itsEditTextFileCmdInput->GetText()->GetText();
	*editTextFileLineCmd = itsEditTextFileLineCmdInput->GetText()->GetText();

	*editBinaryLocally   = !itsExtBinaryEditorCB->IsChecked();
	*editBinaryFileCmd   = itsEditBinaryFileCmdInput->GetText()->GetText();
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
