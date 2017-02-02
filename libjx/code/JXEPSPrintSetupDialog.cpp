/******************************************************************************
 JXEPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXEPSPrintSetupDialog.h>
#include <JXEPSPrinter.h>
#include <JXPSPrintSetupDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXFileInput.h>
#include <jXGlobals.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXEPSPrintSetupDialog*
JXEPSPrintSetupDialog::Create
	(
	const JString&	fileName,
	const JBoolean	printPreview,
	const JBoolean	bw
	)
{
	JXEPSPrintSetupDialog* dlog = jnew JXEPSPrintSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(fileName, printPreview, bw);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXEPSPrintSetupDialog::JXEPSPrintSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEPSPrintSetupDialog::~JXEPSPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXEPSPrintSetupDialog::BuildWindow
	(
	const JString&	fileName,
	const JBoolean	printPreview,
	const JBoolean	bw
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,130, JString::empty);
	assert( window != NULL );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( itsChooseFileButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,100, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXEPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,100, 70,20);
	assert( cancelButton != NULL );

	itsBWCheckbox =
		jnew JXTextCheckbox(JGetString("itsBWCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,60, 150,20);
	assert( itsBWCheckbox != NULL );

	itsPreviewCheckbox =
		jnew JXTextCheckbox(JGetString("itsPreviewCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,60, 130,20);
	assert( itsPreviewCheckbox != NULL );

	itsFileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,20, 250,20);
	assert( itsFileInput != NULL );

// end JXLayout

	SetObjects(okButton, cancelButton, itsFileInput, fileName, itsChooseFileButton,
			   itsPreviewCheckbox, printPreview, itsBWCheckbox, bw);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXEPSPrintSetupDialog::SetObjects
	(
	JXTextButton*	okButton,
	JXTextButton*	cancelButton,
	JXFileInput*	fileInput,
	const JString&	fileName,
	JXTextButton*	chooseFileButton,
	JXTextCheckbox*	previewCheckbox,
	const JBoolean	printPreview,
	JXTextCheckbox*	bwCheckbox,
	const JBoolean	bw
	)
{
	itsPrintButton      = okButton;
	itsFileInput        = fileInput;
	itsChooseFileButton = chooseFileButton;
	itsPreviewCheckbox  = previewCheckbox;
	itsBWCheckbox       = bwCheckbox;

	(itsChooseFileButton->GetWindow())->SetTitle(JGetString("WindowTitle::JXEPSPrintSetupDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsChooseFileButton);

	itsPreviewCheckbox->SetState(printPreview);
	itsBWCheckbox->SetState(bw);

	itsChooseFileButton->SetShortcuts(JGetString("ChooseFileShortcut::JXEPSPrintSetupDialog"));
	itsBWCheckbox->SetShortcuts(JGetString("BlackWhiteShortcut::JXEPSPrintSetupDialog"));
	itsPreviewCheckbox->SetShortcuts(JGetString("PreviewShortcut::JXEPSPrintSetupDialog"));

	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->SetText(fileName);
	itsFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	if (itsFileInput->IsEmpty())
		{
		ChooseDestinationFile();
		}

	UpdateDisplay();
	ListenTo(itsFileInput);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXEPSPrintSetupDialog::UpdateDisplay()
{
	itsPrintButton->SetActive(!itsFileInput->IsEmpty());
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXEPSPrintSetupDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	return JXPSPrintSetupDialog::OKToDeactivate(itsFileInput->GetText());
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXEPSPrintSetupDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
		{
		ChooseDestinationFile();
		}
	else if (sender == itsFileInput &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateDisplay();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseDestinationFile (private)

 ******************************************************************************/

void
JXEPSPrintSetupDialog::ChooseDestinationFile()
{
	itsFileInput->SaveFile(JGetString("SaveEPSPrompt::JXEPSPrintSetupDialog"));
}

/******************************************************************************
 SetParameters

 ******************************************************************************/

JBoolean
JXEPSPrintSetupDialog::SetParameters
	(
	JXEPSPrinter* p
	)
	const
{
	const JBoolean changed = JI2B(
		itsFileInput->GetText()         != p->GetOutputFileName() ||
		itsPreviewCheckbox->IsChecked() != p->WantsPreview()      ||
		itsBWCheckbox->IsChecked()      != p->PSWillPrintBlackWhite());

	JString fullName;
	itsFileInput->GetFile(&fullName);

	p->SetOutputFileName(fullName);
	p->ShouldPrintPreview(itsPreviewCheckbox->IsChecked());
	p->PSPrintBlackWhite(itsBWCheckbox->IsChecked());

	return changed;
}
