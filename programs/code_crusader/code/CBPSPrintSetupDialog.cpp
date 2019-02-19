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
#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBPSPrintSetupDialog*
CBPSPrintSetupDialog::Create
	(
	const JXPSPrinter::Destination	dest,
	const JString&					printCmd,
	const JString&					fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
	CBPSPrintSetupDialog* dlog = jnew CBPSPrintSetupDialog;
	assert( dlog != nullptr );
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
	const JString&					printCmd,
	const JString&					fileName,
	const JBoolean					collate,
	const JBoolean					bw,
	const JSize						fontSize,
	const JBoolean					printHeader
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,300, JString::empty);
	assert( window != nullptr );

	JXStaticText* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 90,19);
	assert( printCmdLabel != nullptr );
	printCmdLabel->SetToLabel();

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,270, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,270, 70,20);
	assert( cancelButton != nullptr );

	JXStaticText* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( destinationLabel != nullptr );
	destinationLabel->SetToLabel();

	JXRadioGroup* destination =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != nullptr );

	JXTextRadioButton* printerRB =
		jnew JXTextRadioButton(1, JGetString("printerRB::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( printerRB != nullptr );
	printerRB->SetShortcuts(JGetString("printerRB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* fileRB =
		jnew JXTextRadioButton(2, JGetString("fileRB::CBPSPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( fileRB != nullptr );
	fileRB->SetShortcuts(JGetString("fileRB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	JXInputField* printCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 250,20);
	assert( printCmdInput != nullptr );

	JXTextButton* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,90, 80,20);
	assert( chooseFileButton != nullptr );

	JXIntegerInput* copyCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 170,110, 40,20);
	assert( copyCount != nullptr );

	JXTextCheckbox* bwCheckbox =
		jnew JXTextCheckbox(JGetString("bwCheckbox::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,190, 150,20);
	assert( bwCheckbox != nullptr );

	JXIntegerInput* firstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,150, 40,20);
	assert( firstPageIndex != nullptr );

	JXIntegerInput* lastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,150, 40,20);
	assert( lastPageIndex != nullptr );

	JXTextCheckbox* printAllCB =
		jnew JXTextCheckbox(JGetString("printAllCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,150, 120,20);
	assert( printAllCB != nullptr );

	JXStaticText* firstPageIndexLabel =
		jnew JXStaticText(JGetString("firstPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,150, 70,20);
	assert( firstPageIndexLabel != nullptr );
	firstPageIndexLabel->SetToLabel();

	JXStaticText* lastPageIndexLabel =
		jnew JXStaticText(JGetString("lastPageIndexLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,150, 20,20);
	assert( lastPageIndexLabel != nullptr );
	lastPageIndexLabel->SetToLabel();

	JXStaticText* copiesLabel =
		jnew JXStaticText(JGetString("copiesLabel::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 55,110, 115,20);
	assert( copiesLabel != nullptr );
	copiesLabel->SetToLabel();

	JXTextCheckbox* collateCB =
		jnew JXTextCheckbox(JGetString("collateCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 240,110, 70,20);
	assert( collateCB != nullptr );

	itsPrintHeaderCB =
		jnew JXTextCheckbox(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,190, 130,20);
	assert( itsPrintHeaderCB != nullptr );
	itsPrintHeaderCB->SetShortcuts(JGetString("itsPrintHeaderCB::CBPSPrintSetupDialog::shortcuts::JXLayout"));

	itsFontSizeMenu =
		jnew JXFontSizeMenu(JFontManager::GetDefaultMonospaceFontName(), JGetString("FontSizeMenuTitle::CBPSPrintSetupDialog"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,220, 170,30);
	assert( itsFontSizeMenu != nullptr );

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
