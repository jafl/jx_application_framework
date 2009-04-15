/******************************************************************************
 JXEPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXEPSPrintSetupDialog.h>
#include <JXEPSPrinter.h>
#include <JXPSPrintSetupDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXFileInput.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXEPSPrintSetupDialog*
JXEPSPrintSetupDialog::Create
	(
	const JCharacter*	fileName,
	const JBoolean		printPreview,
	const JBoolean		bw
	)
{
	JXEPSPrintSetupDialog* dlog = new JXEPSPrintSetupDialog;
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
	const JCharacter*	fileName,
	const JBoolean		printPreview,
	const JBoolean		bw
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,130, "");
    assert( window != NULL );
    SetWindow(window);

    itsChooseFileButton =
        new JXTextButton(JGetString("itsChooseFileButton::JXEPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
    assert( itsChooseFileButton != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXEPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 229,99, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXEPSPrintSetupDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXEPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,100, 70,20);
    assert( cancelButton != NULL );

    itsBWCheckbox =
        new JXTextCheckbox(JGetString("itsBWCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 190,60, 150,20);
    assert( itsBWCheckbox != NULL );

    itsPreviewCheckbox =
        new JXTextCheckbox(JGetString("itsPreviewCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,60, 130,20);
    assert( itsPreviewCheckbox != NULL );

    itsFileInput =
        new JXFileInput(window,
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
	JXTextButton*		okButton,
	JXTextButton*		cancelButton,
	JXFileInput*		fileInput,
	const JCharacter*	fileName,
	JXTextButton*		chooseFileButton,
	JXTextCheckbox*		previewCheckbox,
	const JBoolean		printPreview,
	JXTextCheckbox*		bwCheckbox,
	const JBoolean		bw
	)
{
	itsPrintButton      = okButton;
	itsFileInput        = fileInput;
	itsChooseFileButton = chooseFileButton;
	itsPreviewCheckbox  = previewCheckbox;
	itsBWCheckbox       = bwCheckbox;

	(itsChooseFileButton->GetWindow())->SetTitle("Print Setup");
	SetButtons(okButton, cancelButton);

	ListenTo(itsChooseFileButton);

	itsPreviewCheckbox->SetState(printPreview);
	itsBWCheckbox->SetState(bw);

    itsChooseFileButton->SetShortcuts("#O");
    itsBWCheckbox->SetShortcuts("#B");
    itsPreviewCheckbox->SetShortcuts("#P");

	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->SetText(fileName);
	if (itsFileInput->IsEmpty())
		{
		ChooseDestinationFile();
		}
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
	if (itsFileInput->SaveFile("Save EPS file as:"))
		{
		itsPrintButton->Activate();
		}
	else if (itsFileInput->IsEmpty())
		{
		itsPrintButton->Deactivate();
		}
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

	p->SetOutputFileName(itsFileInput->GetText());
	p->ShouldPrintPreview(itsPreviewCheckbox->IsChecked());
	p->PSPrintBlackWhite(itsBWCheckbox->IsChecked());

	return changed;
}
