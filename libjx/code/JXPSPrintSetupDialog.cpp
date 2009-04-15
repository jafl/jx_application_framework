/******************************************************************************
 JXPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPSPrintSetupDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXFileInput.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXFileNameDisplay.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

// mappings from dialog window to JPSPrinter options

enum
{
	kPrintToPrinterID = 1,
	kPrintToFileID
};
static const JXPSPrinter::Destination kIndexToDest[] =
{
	JXPSPrinter::kPrintToPrinter,
	JXPSPrinter::kPrintToFile
};
static const JSize kDestCount = sizeof(kIndexToDest)/sizeof(JXPSPrinter::Destination);

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPSPrintSetupDialog*
JXPSPrintSetupDialog::Create
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw
	)
{
	JXPSPrintSetupDialog* dlog = new JXPSPrintSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(dest, printCmd, fileName, collate, bw);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPSPrintSetupDialog::JXPSPrintSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPSPrintSetupDialog::~JXPSPrintSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPSPrintSetupDialog::BuildWindow
	(
	const JXPSPrinter::Destination	dest,
	const JCharacter*				printCmd,
	const JCharacter*				fileName,
	const JBoolean					collate,
	const JBoolean					bw
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,250, "");
    assert( window != NULL );
    SetWindow(window);

    itsPrintCmdLabel =
        new JXStaticText(JGetString("itsPrintCmdLabel::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 100,19);
    assert( itsPrintCmdLabel != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 219,219, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,220, 70,20);
    assert( cancelButton != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,30, 80,20);
    assert( obj1_JXLayout != NULL );

    itsDestination =
        new JXRadioGroup(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 139,39);
    assert( itsDestination != NULL );

    JXTextRadioButton* obj2_JXLayout =
        new JXTextRadioButton(1, JGetString("obj2_JXLayout::JXPSPrintSetupDialog::JXLayout"), itsDestination,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,8, 70,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetShortcuts(JGetString("obj2_JXLayout::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    JXTextRadioButton* obj3_JXLayout =
        new JXTextRadioButton(2, JGetString("obj3_JXLayout::JXPSPrintSetupDialog::JXLayout"), itsDestination,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 75,8, 50,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetShortcuts(JGetString("obj3_JXLayout::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    itsPrintCmd =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,70, 240,20);
    assert( itsPrintCmd != NULL );

    itsChooseFileButton =
        new JXTextButton(JGetString("itsChooseFileButton::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,70, 80,20);
    assert( itsChooseFileButton != NULL );
    itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    itsCopyCount =
        new JXIntegerInput(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,110, 40,20);
    assert( itsCopyCount != NULL );

    itsBWCheckbox =
        new JXTextCheckbox(JGetString("itsBWCheckbox::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,190, 150,20);
    assert( itsBWCheckbox != NULL );
    itsBWCheckbox->SetShortcuts(JGetString("itsBWCheckbox::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    itsFirstPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 230,150, 40,20);
    assert( itsFirstPageIndex != NULL );

    itsLastPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 290,150, 40,20);
    assert( itsLastPageIndex != NULL );

    itsPrintAllCB =
        new JXTextCheckbox(JGetString("itsPrintAllCB::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,150, 120,20);
    assert( itsPrintAllCB != NULL );
    itsPrintAllCB->SetShortcuts(JGetString("itsPrintAllCB::JXPSPrintSetupDialog::shortcuts::JXLayout"));

    itsFirstPageIndexLabel =
        new JXStaticText(JGetString("itsFirstPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 163,153, 66,20);
    assert( itsFirstPageIndexLabel != NULL );

    itsLastPageIndexLabel =
        new JXStaticText(JGetString("itsLastPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 273,153, 16,20);
    assert( itsLastPageIndexLabel != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 55,110, 115,20);
    assert( obj4_JXLayout != NULL );

    itsCollateCB =
        new JXTextCheckbox(JGetString("itsCollateCB::JXPSPrintSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 240,110, 70,20);
    assert( itsCollateCB != NULL );
    itsCollateCB->SetShortcuts(JGetString("itsCollateCB::JXPSPrintSetupDialog::shortcuts::JXLayout"));

// end JXLayout

	SetObjects(okButton, cancelButton, itsDestination, dest,
			   itsPrintCmdLabel, itsPrintCmd, printCmd,
			   itsChooseFileButton, fileName, itsCopyCount,
			   itsCollateCB, collate, itsBWCheckbox, bw,
			   itsPrintAllCB, itsFirstPageIndexLabel,
			   itsFirstPageIndex, itsLastPageIndexLabel, itsLastPageIndex);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXPSPrintSetupDialog::SetObjects
	(
	JXTextButton*					okButton,
	JXTextButton*					cancelButton,
	JXRadioGroup*					destinationRG,
	const JXPSPrinter::Destination	dest,
	JXStaticText*					printCmdLabel,
	JXInputField*					printCmdInput,
	const JCharacter*				printCmd,
	JXTextButton*					chooseFileButton,
	const JCharacter*				fileName,
	JXIntegerInput*					copyCount,
	JXTextCheckbox*					collateCheckbox,
	const JBoolean					collate,
	JXTextCheckbox*					bwCheckbox,
	const JBoolean					bw,
	JXTextCheckbox*					printAllCheckbox,
	JXStaticText*					firstPageIndexLabel,
	JXIntegerInput*					firstPageIndex,
	JXStaticText*					lastPageIndexLabel,
	JXIntegerInput*					lastPageIndex
	)
{
	itsPrintButton         = okButton;
	itsDestination         = destinationRG;
	itsPrintCmdLabel       = printCmdLabel;
	itsPrintCmd            = printCmdInput;
	itsChooseFileButton    = chooseFileButton;
	itsCopyCount           = copyCount;
	itsCollateCB           = collateCheckbox;
	itsBWCheckbox          = bwCheckbox;
	itsPrintAllCB          = printAllCheckbox;
	itsFirstPageIndexLabel = firstPageIndexLabel;
	itsFirstPageIndex      = firstPageIndex;
	itsLastPageIndexLabel  = lastPageIndexLabel;
	itsLastPageIndex       = lastPageIndex;

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

	itsCollateCB->SetState(collate);
	itsBWCheckbox->SetState(bw);

    itsChooseFileButton->SetShortcuts("#O");
    itsBWCheckbox->SetShortcuts("#B");
    itsPrintAllCB->SetShortcuts("#L");
    itsCollateCB->SetShortcuts("#E");
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXPSPrintSetupDialog::OKToDeactivate()
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
		return OKToDeactivate(itsFileInput->GetText());
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 OKToDeactivate (static)

 ******************************************************************************/

JBoolean
JXPSPrintSetupDialog::OKToDeactivate
	(
	const JString& fullName
	)
{
	if (fullName.IsEmpty())
		{
		(JGetUserNotification())->ReportError(
			"Please specify a destination file.");
		return kJFalse;
		}

	JString path, fileName;
	JSplitPathAndName(fullName, &path, &fileName);
	const JBoolean fileExists = JFileExists(fullName);
	if (!JDirectoryExists(path))
		{
		(JGetUserNotification())->ReportError(
			"The specified directory does not exist.");
		return kJFalse;
		}
	else if (!fileExists && !JDirectoryWritable(path))
		{
		(JGetUserNotification())->ReportError(
			"The file cannot be created because the you do not have write access "
			"to the specified directory.");
		return kJFalse;
		}
	else if (fileExists && !JFileWritable(fullName))
		{
		(JGetUserNotification())->ReportError(
			"You do not have write access to the specified file.");
		return kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXPSPrintSetupDialog::Receive
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
JXPSPrintSetupDialog::SetDestination
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
		itsCollateCB->Show();
		itsChooseFileButton->Hide();
		itsFileInput->Hide();
		}
	else
		{
		assert( id == kPrintToFileID );

		itsPrintCmdLabel->Hide();
		itsPrintCmd->Hide();
		itsPrintCmd->SetIsRequired(kJFalse);
		itsCollateCB->Hide();
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
JXPSPrintSetupDialog::PrintAllPages
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
JXPSPrintSetupDialog::ChooseDestinationFile()
{
	if (itsFileInput->SaveFile("Save PostScript file as:"))
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
JXPSPrintSetupDialog::SetParameters
	(
	JXPSPrinter* p
	)
	const
{
	const JXPSPrinter::Destination newDest =
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
		itsCollateCB->IsChecked()  != p->WillCollatePages() ||
		itsBWCheckbox->IsChecked() != p->WillPrintBlackWhite());

	p->CollatePages(itsCollateCB->IsChecked());
	p->PSPrintBlackWhite(itsBWCheckbox->IsChecked());

	return changed;
}
