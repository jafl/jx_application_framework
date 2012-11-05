/******************************************************************************
 CBExtEditorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
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

	JXWindow* window = new JXWindow(this, 450,220, "");
	assert( window != NULL );

	itsEditTextFileCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 230,20);
	assert( itsEditTextFileCmdInput != NULL );

	itsEditTextFileLineCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,60, 230,20);
	assert( itsEditTextFileLineCmdInput != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,190, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,190, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBExtEditorDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,80, 270,20);
	assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetFontSize(8);
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,40, 170,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,60, 170,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	itsExtTextEditorCB =
		new JXTextCheckbox(JGetString("itsExtTextEditorCB::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 160,20);
	assert( itsExtTextEditorCB != NULL );

	itsEditBinaryFileCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,130, 230,20);
	assert( itsEditBinaryFileCmdInput != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,150, 230,20);
	assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(8);
	obj4_JXLayout->SetToLabel();

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::CBExtEditorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,130, 170,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	itsExtBinaryEditorCB =
		new JXTextCheckbox(JGetString("itsExtBinaryEditorCB::CBExtEditorDialog::JXLayout"), window,
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
