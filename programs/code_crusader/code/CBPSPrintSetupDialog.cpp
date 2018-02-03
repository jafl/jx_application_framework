/******************************************************************************
 CBPSPrintSetupDialog.cpp

	BASE CLASS = JXPSPrintSetupDialog

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBPSPrintSetupDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFontSizeMenu.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBPSPrintSetupDialog*
CBPSPrintSetupDialog::Create
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
	CBPSPrintSetupDialog* dlog = jnew CBPSPrintSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(dest, printCmd, fileName, collate, bw, fontSize, printHeader);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPSPrintSetupDialog::CBPSPrintSetupDialog()
	:
	JXPSPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPSPrintSetupDialog::~CBPSPrintSetupDialog()
{
}

/******************************************************************************
 CBGetSettings

 ******************************************************************************/

void
CBPSPrintSetupDialog::CBGetSettings
	(
	JSize*		fontSize,
	JBoolean*	printHeader
	)
	const
{
	*fontSize    = itsFontSizeMenu->GetFontSize();
	*printHeader = itsPrintHeaderCB->IsChecked();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBPSPrintSetupDialog::BuildWindow
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,300, "");
	assert( window != NULL );

	JXStaticText* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 100,19);
	assert( printCmdLabel != NULL );
	printCmdLabel->SetToLabel();

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,270, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,270, 70,20);
	assert( cancelButton != NULL );

	JXStaticText* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( destinationLabel != NULL );
	destinationLabel->SetToLabel();

	JXRadioGroup* destination =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != NULL );

	JXTextRadioButton* printerRB =
		jnew JXTextRadioButton(1, JGetString("printerRB::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( printerRB != NULL );
	printerRB->SetShortcuts(JGetString("printerRB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* fileRB =
		jnew JXTextRadioButton(2, JGetString("fileRB::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( fileRB != NULL );
	fileRB->SetShortcuts(JGetString("fileRB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXInputField* printCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 110,70, 240,20);
	assert( printCmdInput != NULL );

	JXTextButton* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,70, 80,20);
	assert( chooseFileButton != NULL );

	JXIntegerInput* copyCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 170,110, 40,20);
	assert( copyCount != NULL );

	JXTextCheckbox* bwCheckbox =
		jnew JXTextCheckbox(JGetString("bwCheckbox::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,190, 150,20);
	assert( bwCheckbox != NULL );

	JXIntegerInput* firstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,150, 40,20);
	assert( firstPageIndex != NULL );

	JXIntegerInput* lastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,150, 40,20);
	assert( lastPageIndex != NULL );

	JXTextCheckbox* printAllCB =
		jnew JXTextCheckbox(JGetString("printAllCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,150, 120,20);
	assert( printAllCB != NULL );

	JXStaticText* firstPageIndexLabel =
		jnew JXStaticText(JGetString("firstPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,150, 70,20);
	assert( firstPageIndexLabel != NULL );
	firstPageIndexLabel->SetToLabel();

	JXStaticText* lastPageIndexLabel =
		jnew JXStaticText(JGetString("lastPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,150, 20,20);
	assert( lastPageIndexLabel != NULL );
	lastPageIndexLabel->SetToLabel();

	JXStaticText* copiesLabel =
		jnew JXStaticText(JGetString("copiesLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 55,110, 115,20);
	assert( copiesLabel != NULL );
	copiesLabel->SetToLabel();

	JXTextCheckbox* collateCB =
		jnew JXTextCheckbox(JGetString("collateCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 240,110, 70,20);
	assert( collateCB != NULL );

	itsPrintHeaderCB =
		jnew JXTextCheckbox(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,190, 130,20);
	assert( itsPrintHeaderCB != NULL );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JGetMonospaceFontName(), "Font size:", window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,220, 170,30);
	assert( itsFontSizeMenu != NULL );

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   collateCB, collate, bwCheckbox, bw,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex);

	itsFontSizeMenu->SetFontSize(fontSize);
	itsFontSizeMenu->SetToPopupChoice();

	itsPrintHeaderCB->SetState(printHeader);
}
