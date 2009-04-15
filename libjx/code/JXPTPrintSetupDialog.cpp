/******************************************************************************
 JXPTPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPTPrintSetupDialog.h>
#include <JXPSPrintSetupDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXFileInput.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

// mappings from dialog window to JPTPrinter options

enum
{
	kPrintToPrinterID = 1,
	kPrintToFileID
};
static const JXPTPrinter::Destination kIndexToDest[] =
{
	JXPTPrinter::kPrintToPrinter,
	JXPTPrinter::kPrintToFile
};
static const JSize kDestCount = sizeof(kIndexToDest)/sizeof(JXPTPrinter::Destination);

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPTPrintSetupDialog*
JXPTPrintSetupDialog::Create
	(
	const JXPTPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					printLineNumbers
	)
{
	JXPTPrintSetupDialog* dlog = new JXPTPrintSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(dest, printCmd, fileName, printLineNumbers);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPTPrintSetupDialog::JXPTPrintSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPTPrintSetupDialog::~JXPTPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::BuildWindow
	(
	const JXPTPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					printLineNumbers
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,220, "");
    assert( window != NULL );
    SetWindow(window);

    itsPrintCmdLabel =
        new JXStaticText(JGetString("itsPrintCmdLabel::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 100,19);
    assert( itsPrintCmdLabel != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 219,189, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXPTPrintSetupDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,190, 70,20);
    assert( cancelButton != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,30, 80,20);
    assert( obj1_JXLayout != NULL );

    itsDestination =
        new JXRadioGroup(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 139,39);
    assert( itsDestination != NULL );

    JXTextRadioButton* obj2_JXLayout =
        new JXTextRadioButton(1, JGetString("obj2_JXLayout::JXPTPrintSetupDialog::JXLayout"), itsDestination,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,8, 70,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetShortcuts(JGetString("obj2_JXLayout::JXPTPrintSetupDialog::shortcuts::JXLayout"));

    JXTextRadioButton* obj3_JXLayout =
        new JXTextRadioButton(2, JGetString("obj3_JXLayout::JXPTPrintSetupDialog::JXLayout"), itsDestination,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 75,8, 50,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetShortcuts(JGetString("obj3_JXLayout::JXPTPrintSetupDialog::shortcuts::JXLayout"));

    itsPrintCmd =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,70, 240,20);
    assert( itsPrintCmd != NULL );

    itsChooseFileButton =
        new JXTextButton(JGetString("itsChooseFileButton::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,70, 80,20);
    assert( itsChooseFileButton != NULL );
    itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::JXPTPrintSetupDialog::shortcuts::JXLayout"));

    itsCopyCount =
        new JXIntegerInput(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,110, 40,20);
    assert( itsCopyCount != NULL );

    itsFirstPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 230,150, 40,20);
    assert( itsFirstPageIndex != NULL );

    itsLastPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 290,150, 40,20);
    assert( itsLastPageIndex != NULL );

    itsPrintAllCB =
        new JXTextCheckbox(JGetString("itsPrintAllCB::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,150, 120,20);
    assert( itsPrintAllCB != NULL );
    itsPrintAllCB->SetShortcuts(JGetString("itsPrintAllCB::JXPTPrintSetupDialog::shortcuts::JXLayout"));

    itsFirstPageIndexLabel =
        new JXStaticText(JGetString("itsFirstPageIndexLabel::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 163,153, 66,20);
    assert( itsFirstPageIndexLabel != NULL );

    itsLastPageIndexLabel =
        new JXStaticText(JGetString("itsLastPageIndexLabel::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 273,153, 16,20);
    assert( itsLastPageIndexLabel != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,110, 115,20);
    assert( obj4_JXLayout != NULL );

    itsPrintLineNumbersCB =
        new JXTextCheckbox(JGetString("itsPrintLineNumbersCB::JXPTPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 220,110, 130,20);
    assert( itsPrintLineNumbersCB != NULL );
    itsPrintLineNumbersCB->SetShortcuts(JGetString("itsPrintLineNumbersCB::JXPTPrintSetupDialog::shortcuts::JXLayout"));

// end JXLayout

	SetObjects(okButton, cancelButton, itsDestination, dest,
			   itsPrintCmdLabel, itsPrintCmd, printCmd,
			   itsChooseFileButton, fileName, itsCopyCount,
			   itsPrintAllCB, itsFirstPageIndexLabel,
			   itsFirstPageIndex, itsLastPageIndexLabel, itsLastPageIndex,
			   itsPrintLineNumbersCB, printLineNumbers);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXPTPrintSetupDialog::SetObjects
	(
	JXTextButton*					okButton,
	JXTextButton*					cancelButton,
	JXRadioGroup*					destinationRG,
	const JXPTPrinter::Destination	dest,
	JXStaticText*					printCmdLabel,
	JXInputField*					printCmdInput,
	const JCharacter*				printCmd,
	JXTextButton*					chooseFileButton,
	const JCharacter*				fileName,
	JXIntegerInput*					copyCount,
	JXTextCheckbox*					printAllCB,
	JXStaticText*					firstPageIndexLabel,
	JXIntegerInput*					firstPageIndex,
	JXStaticText*					lastPageIndexLabel,
	JXIntegerInput*					lastPageIndex,
	JXTextCheckbox*					printLineNumbersCB,
	const JBoolean					printLineNumbers
	)
{
	itsPrintButton         = okButton;
	itsDestination         = destinationRG;
	itsPrintCmdLabel       = printCmdLabel;
	itsPrintCmd            = printCmdInput;
	itsChooseFileButton    = chooseFileButton;
	itsCopyCount           = copyCount;
	itsPrintAllCB          = printAllCB;
	itsFirstPageIndexLabel = firstPageIndexLabel;
	itsFirstPageIndex      = firstPageIndex;
	itsLastPageIndexLabel  = lastPageIndexLabel;
	itsLastPageIndex       = lastPageIndex;
	itsPrintLineNumbersCB  = printLineNumbersCB;

	JXWindow* window = itsDestination->GetWindow();
	window->SetTitle("Print Setup");
	SetButtons(okButton, cancelButton);

	const JRect r = itsPrintCmd->GetFrame();
	itsFileInput =
		new JXFileInput(window,
						JXWidget::kHElastic, JXWidget::kVElastic,
						r.left, r.top, r.width(), r.height());
	assert( itsFileInput != NULL );
	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->SetText(fileName);

	itsPrintCmd->SetText(printCmd);
	itsPrintCmd->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	ListenTo(itsDestination);
	ListenTo(itsChooseFileButton);
	ListenTo(itsPrintAllCB);

	JBoolean foundDest = kJFalse;
	for (JIndex i=1; i<=kDestCount; i++)
		{
		if (kIndexToDest[i-1] == dest)
			{
			SetDestination(i);
			foundDest = kJTrue;
			break;
			}
		}
	assert( foundDest );

	itsCopyCount->SetValue(1);
	itsCopyCount->SetLowerLimit(1);

	PrintAllPages(kJTrue);

	itsFirstPageIndex->SetLowerLimit(1);
	itsFirstPageIndex->SetIsRequired(kJFalse);

	itsLastPageIndex->SetLowerLimit(1);
	itsLastPageIndex->SetIsRequired(kJFalse);

	itsPrintLineNumbersCB->SetState(printLineNumbers);

    itsChooseFileButton->SetShortcuts("#O");
    itsPrintAllCB->SetShortcuts("#L");
    itsPrintLineNumbersCB->SetShortcuts("#N");
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXPTPrintSetupDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	if (itsDestination->GetSelectedItem() == kPrintToFileID)
		{
		return JXPSPrintSetupDialog::OKToDeactivate(itsFileInput->GetText());
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXPTPrintSetupDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDestination && message.Is(JXRadioGroup::kSelectionChanged))
		{
		const JXRadioGroup::SelectionChanged* selection =
			dynamic_cast(const JXRadioGroup::SelectionChanged*, &message);
		assert( selection != NULL );
		SetDestination(selection->GetID());
		}

	else if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
		{
		ChooseDestinationFile();
		}

	else if (sender == itsPrintAllCB && message.Is(JXCheckbox::kPushed))
		{
		PrintAllPages(itsPrintAllCB->IsChecked());
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 SetDestination (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::SetDestination
	(
	const JIndex id
	)
{
	itsDestination->SelectItem(id);

	if (id == kPrintToPrinterID)
		{
		itsPrintButton->Activate();
		itsPrintCmdLabel->Show();
		itsPrintCmd->Show();
		itsPrintCmd->SetIsRequired(kJTrue);
		itsPrintCmd->Focus();
		itsChooseFileButton->Hide();
		itsFileInput->Hide();
		}
	else
		{
		assert( id == kPrintToFileID );

		itsPrintCmdLabel->Hide();
		itsPrintCmd->Hide();
		itsPrintCmd->SetIsRequired(kJFalse);
		itsChooseFileButton->Show();
		itsFileInput->Show();
		itsFileInput->Focus();

		if (itsFileInput->IsEmpty())
			{
			ChooseDestinationFile();
			}
		}
}

/******************************************************************************
 PrintAllPages (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::PrintAllPages
	(
	const JBoolean all
	)
{
	itsPrintAllCB->SetState(all);

	if (all)
		{
		if (itsFirstPageIndex->HasFocus() || itsLastPageIndex->HasFocus())
			{
			(GetWindow())->KillFocus();		// values become irrelevant
			(GetWindow())->SwitchFocusToFirstWidget();
			}
		itsFirstPageIndex->Hide();
		itsLastPageIndex->Hide();
		itsFirstPageIndexLabel->Hide();
		itsLastPageIndexLabel->Hide();
		}
	else
		{
		itsFirstPageIndex->Show();
		itsFirstPageIndex->Focus();
		itsLastPageIndex->Show();
		itsFirstPageIndexLabel->Show();
		itsLastPageIndexLabel->Show();
		}
}

/******************************************************************************
 ChooseDestinationFile (private)

 ******************************************************************************/

void
JXPTPrintSetupDialog::ChooseDestinationFile()
{
	if (itsFileInput->SaveFile("Save text file as:"))
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
JXPTPrintSetupDialog::SetParameters
	(
	JXPTPrinter* p
	)
	const
{
	const JXPTPrinter::Destination newDest =
		kIndexToDest[ itsDestination->GetSelectedItem()-1 ];

	JBoolean changed = JI2B(
		newDest                 != p->GetDestination() ||
		itsPrintCmd->GetText()  != p->GetPrintCmd()    ||
		itsFileInput->GetText() != p->GetFileName());

	p->SetDestination(newDest, itsPrintCmd->GetText(), itsFileInput->GetText());

	JInteger copyCount;
	const JBoolean ok = itsCopyCount->GetValue(&copyCount);
	assert( ok );
	p->SetCopyCount(copyCount);

	const JBoolean printAll = itsPrintAllCB->IsChecked();
	p->PrintAllPages();

	if (!printAll)
		{
		JInteger p1, p2;
		const JBoolean ok1 = itsFirstPageIndex->GetValue(&p1);
		const JBoolean ok2 = itsLastPageIndex->GetValue(&p2);
		if (ok1 && ok2)
			{
			p->SetFirstPageToPrint(JMin(p1, p2));
			p->SetLastPageToPrint(JMax(p1, p2));
			}
		else
			{
			if (ok1)
				{
				p->SetFirstPageToPrint(p1);
				}
			if (ok2)
				{
				p->SetLastPageToPrint(p2);
				}
			}
		}

	changed = JI2B(changed ||
		itsPrintLineNumbersCB->IsChecked() != p->WillPrintLineNumbers());

	p->ShouldPrintLineNumbers(itsPrintLineNumbersCB->IsChecked());

	return changed;
}
