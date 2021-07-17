/******************************************************************************
 JXEPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#include "JXEPSPrintSetupDialog.h"
#include "JXEPSPrinter.h"
#include "JXPSPrintSetupDialog.h"
#include "JXChooseEPSDestFileTask.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXTextCheckbox.h"
#include "JXFileInput.h"
#include "jXGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXEPSPrintSetupDialog*
JXEPSPrintSetupDialog::Create
	(
	const JString&	fileName,
	const bool	printPreview,
	const bool	bw
	)
{
	JXEPSPrintSetupDialog* dlog = jnew JXEPSPrintSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(fileName, printPreview, bw);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXEPSPrintSetupDialog::JXEPSPrintSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), true)
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
	const bool	printPreview,
	const bool	bw
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,130, JString::empty);
	assert( window != nullptr );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( itsChooseFileButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,100, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXEPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,100, 70,20);
	assert( cancelButton != nullptr );

	itsBWCheckbox =
		jnew JXTextCheckbox(JGetString("itsBWCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,60, 150,20);
	assert( itsBWCheckbox != nullptr );

	itsPreviewCheckbox =
		jnew JXTextCheckbox(JGetString("itsPreviewCheckbox::JXEPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,60, 130,20);
	assert( itsPreviewCheckbox != nullptr );

	itsFileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,20, 250,20);
	assert( itsFileInput != nullptr );

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
	const bool	printPreview,
	JXTextCheckbox*	bwCheckbox,
	const bool	bw
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
	itsFileInput->GetText()->SetText(fileName);
	if (itsFileInput->GetText()->IsEmpty())
		{
		JXChooseEPSDestFileTask* task = jnew JXChooseEPSDestFileTask(this);
		assert( task != nullptr );
		task->Go();
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
	itsPrintButton->SetActive(!itsFileInput->GetText()->IsEmpty());
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXEPSPrintSetupDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}

	return JXPSPrintSetupDialog::OKToDeactivate(itsFileInput->GetText()->GetText());
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
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
		{
		UpdateDisplay();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseDestinationFile

 ******************************************************************************/

void
JXEPSPrintSetupDialog::ChooseDestinationFile()
{
	itsFileInput->SaveFile(JGetString("SaveEPSPrompt::JXEPSPrintSetupDialog"));
}

/******************************************************************************
 SetParameters

 ******************************************************************************/

bool
JXEPSPrintSetupDialog::SetParameters
	(
	JXEPSPrinter* p
	)
	const
{
	const bool changed = itsFileInput->GetText()->GetText() != p->GetOutputFileName() ||
		itsPreviewCheckbox->IsChecked()    != p->WantsPreview()      ||
		itsBWCheckbox->IsChecked()         != p->PSWillPrintBlackWhite();

	JString fullName;
	itsFileInput->GetFile(&fullName);

	p->SetOutputFileName(fullName);
	p->ShouldPrintPreview(itsPreviewCheckbox->IsChecked());
	p->PSPrintBlackWhite(itsBWCheckbox->IsChecked());

	return changed;
}
