/******************************************************************************
 GMPTPrintSetupDialog.cc

	BASE CLASS = JXPTPrintSetupDialog

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GMPTPrintSetupDialog.h"
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
 Constructor

 ******************************************************************************/

GMPTPrintSetupDialog::GMPTPrintSetupDialog()
	:
	JXPTPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMPTPrintSetupDialog::~GMPTPrintSetupDialog()
{
}

/******************************************************************************
 PrintHeader

 ******************************************************************************/

JBoolean
GMPTPrintSetupDialog::WillPrintHeader()
	const
{
	return itsPrintHeadersCB->IsChecked();
}

void
GMPTPrintSetupDialog::ShouldPrintHeader
	(
	const JBoolean print
	)
{
	itsPrintHeadersCB->SetState(print);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GMPTPrintSetupDialog::BuildWindow
	(
	const JXPTPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					printLineNumbers,
	const JBoolean					printHeader
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,240, "");
	assert( window != NULL );

	JXStaticText* printCmdLabel =
		new JXStaticText(JGetString("printCmdLabel::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 100,19);
	assert( printCmdLabel != NULL );
	printCmdLabel->SetToLabel();

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,210, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,210, 70,20);
	assert( cancelButton != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXRadioGroup* destination =
		new JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
	assert( destination != NULL );

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(1, JGetString("obj2_JXLayout::GMPTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetShortcuts(JGetString("obj2_JXLayout::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* obj3_JXLayout =
		new JXTextRadioButton(2, JGetString("obj3_JXLayout::GMPTPrintSetupDialog::JXLayout"), destination,
					JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetShortcuts(JGetString("obj3_JXLayout::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXInputField* printCmdInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 110,70, 240,20);
	assert( printCmdInput != NULL );

	JXTextButton* chooseFileButton =
		new JXTextButton(JGetString("chooseFileButton::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,70, 80,20);
	assert( chooseFileButton != NULL );
	chooseFileButton->SetShortcuts(JGetString("chooseFileButton::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXIntegerInput* copyCount =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 140,120, 40,20);
	assert( copyCount != NULL );

	JXIntegerInput* firstPageIndex =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,170, 40,20);
	assert( firstPageIndex != NULL );

	JXIntegerInput* lastPageIndex =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 290,170, 40,20);
	assert( lastPageIndex != NULL );

	JXTextCheckbox* printAllCB =
		new JXTextCheckbox(JGetString("printAllCB::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 40,170, 120,20);
	assert( printAllCB != NULL );
	printAllCB->SetShortcuts(JGetString("printAllCB::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXStaticText* firstPageIndexLabel =
		new JXStaticText(JGetString("firstPageIndexLabel::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,170, 70,20);
	assert( firstPageIndexLabel != NULL );
	firstPageIndexLabel->SetToLabel();

	JXStaticText* lastPageIndexLabel =
		new JXStaticText(JGetString("lastPageIndexLabel::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,170, 20,20);
	assert( lastPageIndexLabel != NULL );
	lastPageIndexLabel->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 25,120, 115,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	itsPrintHeadersCB =
		new JXTextCheckbox(JGetString("itsPrintHeadersCB::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,110, 130,20);
	assert( itsPrintHeadersCB != NULL );
	itsPrintHeadersCB->SetShortcuts(JGetString("itsPrintHeadersCB::GMPTPrintSetupDialog::shortcuts::JXLayout"));

	JXTextCheckbox* printLineNumbersCB =
		new JXTextCheckbox(JGetString("printLineNumbersCB::GMPTPrintSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 220,130, 130,20);
	assert( printLineNumbersCB != NULL );
	printLineNumbersCB->SetShortcuts(JGetString("printLineNumbersCB::GMPTPrintSetupDialog::shortcuts::JXLayout"));

// end JXLayout

	SetObjects(okButton, cancelButton, destination, dest,
			   printCmdLabel, printCmdInput, printCmd,
			   chooseFileButton, fileName, copyCount,
			   printAllCB, firstPageIndexLabel,
			   firstPageIndex, lastPageIndexLabel, lastPageIndex,
			   printLineNumbersCB, printLineNumbers);

	itsPrintHeadersCB->SetState(printHeader);
}
