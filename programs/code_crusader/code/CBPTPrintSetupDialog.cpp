/******************************************************************************
 CBPTPrintSetupDialog.cpp

	BASE CLASS = JXPTPrintSetupDialog

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBPTPrintSetupDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBPTPrintSetupDialog*
CBPTPrintSetupDialog::Create
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool					printLineNumbers,
	const bool					printHeader
	)
{
	auto* dlog = jnew CBPTPrintSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(dest, printCmd, fileName, printLineNumbers, printHeader);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPTPrintSetupDialog::CBPTPrintSetupDialog()
	:
	JXPTPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPTPrintSetupDialog::~CBPTPrintSetupDialog()
{
}

/******************************************************************************
 ShouldPrintHeader

 ******************************************************************************/

bool
CBPTPrintSetupDialog::ShouldPrintHeader()
	const
{
	return itsPrintHeaderCB->IsChecked();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBPTPrintSetupDialog::BuildWindow
	(
	const JXPTPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const bool					printLineNumbers,
	const bool					printHeader
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,240, JString::empty);
	assert( window != nullptr );

	auto* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 90,19);
	assert( printCmdLabel != nullptr );
	printCmdLabel->SetToLabel();

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,210, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBPTPrintSetupDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,210, 70,20);
	assert( cancelButton != nullptr );

	auto* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( destinationLabel != nullptr );
	destinationLabel->SetToLabel();

	auto* destination =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != nullptr );

	auto* printerRB =
		jnew JXTextRadioButton(1, JGetString("printerRB::CBPTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( printerRB != nullptr );
	printerRB->SetShortcuts(JGetString("printerRB::CBPTPrintSetupDialog::shortcuts::JXLayout"));

	auto* fileRB =
		jnew JXTextRadioButton(2, JGetString("fileRB::CBPTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( fileRB != nullptr );
	fileRB->SetShortcuts(JGetString("fileRB::CBPTPrintSetupDialog::shortcuts::JXLayout"));

	auto* printCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 250,20);
	assert( printCmdInput != nullptr );

	auto* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,90, 80,20);
	assert( chooseFileButton != nullptr );
	chooseFileButton->SetShortcuts(JGetString("chooseFileButton::CBPTPrintSetupDialog::shortcuts::JXLayout"));

	auto* copyCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,120, 40,20);
	assert( copyCount != nullptr );

	auto* firstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,170, 40,20);
	assert( firstPageIndex != nullptr );

	auto* lastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,170, 40,20);
	assert( lastPageIndex != nullptr );

	auto* printAllCB =
		jnew JXTextCheckbox(JGetString("printAllCB::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,170, 120,20);
	assert( printAllCB != nullptr );

	auto* firstPageIndexLabel =
		jnew JXStaticText(JGetString("firstPageIndexLabel::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,170, 70,20);
	assert( firstPageIndexLabel != nullptr );
	firstPageIndexLabel->SetToLabel();

	auto* lastPageIndexLabel =
		jnew JXStaticText(JGetString("lastPageIndexLabel::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,170, 20,20);
	assert( lastPageIndexLabel != nullptr );
	lastPageIndexLabel->SetToLabel();

	auto* copiesLabel =
		jnew JXStaticText(JGetString("copiesLabel::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 25,120, 115,20);
	assert( copiesLabel != nullptr );
	copiesLabel->SetToLabel();

	itsPrintHeaderCB =
		jnew JXTextCheckbox(JGetString("itsPrintHeaderCB::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,110, 130,20);
	assert( itsPrintHeaderCB != nullptr );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::CBPTPrintSetupDialog::shortcuts::JXLayout"));

	auto* printLineNumbersCB =
		jnew JXTextCheckbox(JGetString("printLineNumbersCB::CBPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,130, 130,20);
	assert( printLineNumbersCB != nullptr );

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex,
			   printLineNumbersCB, printLineNumbers);

	itsPrintHeaderCB->SetState(printHeader);
}
