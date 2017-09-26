/******************************************************************************
 JXPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXPSPrintSetupDialog.h>
#include <JXAdjustPSPrintSetupLayoutTask.h>
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
#include <jFileUtil.h>
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
	JXPSPrintSetupDialog* dlog = jnew JXPSPrintSetupDialog;
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

	JXWindow* window = jnew JXWindow(this, 370,250, "");
	assert( window != NULL );

	itsPrintCmdLabel =
		jnew JXStaticText(JGetString("itsPrintCmdLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 90,20);
	assert( itsPrintCmdLabel != NULL );
	itsPrintCmdLabel->SetToLabel();

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,220, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,220, 70,20);
	assert( cancelButton != NULL );

	JXStaticText* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,30, 80,20);
	assert( destinationLabel != NULL );
	destinationLabel->SetToLabel();

	itsDestination =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 139,39);
	assert( itsDestination != NULL );

	JXTextRadioButton* printerLabel =
		jnew JXTextRadioButton(1, JGetString("printerLabel::JXPSPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,8, 70,20);
	assert( printerLabel != NULL );
	printerLabel->SetShortcuts(JGetString("printerLabel::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	JXTextRadioButton* fileLabel =
		jnew JXTextRadioButton(2, JGetString("fileLabel::JXPSPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 75,8, 50,20);
	assert( fileLabel != NULL );
	fileLabel->SetShortcuts(JGetString("fileLabel::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	itsPrintCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,70, 250,20);
	assert( itsPrintCmd != NULL );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,90, 90,20);
	assert( itsChooseFileButton != NULL );
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	itsCopyCount =
		jnew JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,110, 40,20);
	assert( itsCopyCount != NULL );

	itsBWCheckbox =
		jnew JXTextCheckbox(JGetString("itsBWCheckbox::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,190, 150,20);
	assert( itsBWCheckbox != NULL );
	itsBWCheckbox->SetShortcuts(JGetString("itsBWCheckbox::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	itsFirstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,150, 40,20);
	assert( itsFirstPageIndex != NULL );

	itsLastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,150, 40,20);
	assert( itsLastPageIndex != NULL );

	itsPrintAllCB =
		jnew JXTextCheckbox(JGetString("itsPrintAllCB::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,150, 110,20);
	assert( itsPrintAllCB != NULL );
	itsPrintAllCB->SetShortcuts(JGetString("itsPrintAllCB::JXPSPrintSetupDialog::shortcuts::JXLayout"));

	itsFirstPageIndexLabel =
		jnew JXStaticText(JGetString("itsFirstPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,150, 70,20);
	assert( itsFirstPageIndexLabel != NULL );
	itsFirstPageIndexLabel->SetToLabel();

	itsLastPageIndexLabel =
		jnew JXStaticText(JGetString("itsLastPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 270,150, 20,20);
	assert( itsLastPageIndexLabel != NULL );
	itsLastPageIndexLabel->SetToLabel();

	JXStaticText* countLabel =
		jnew JXStaticText(JGetString("countLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 55,110, 115,20);
	assert( countLabel != NULL );
	countLabel->SetToLabel();

	itsCollateCB =
		jnew JXTextCheckbox(JGetString("itsCollateCB::JXPSPrintSetupDialog::JXLayout"), window,
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

	const JRect r1 = itsPrintCmd->GetFrame(),
				r2 = itsChooseFileButton->GetFrame();
	itsFileInput =
		jnew JXFileInput(window,
						JXWidget::kHElastic, JXWidget::kVElastic,
						r1.left, r2.top, r1.width(), r2.height());
	assert( itsFileInput != NULL );
	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->SetText(fileName);
	itsFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsFileInput);

	JXAdjustPSPrintSetupLayoutTask* task =
		jnew JXAdjustPSPrintSetupLayoutTask(this, r1.top - r2.top, itsChooseFileButton, itsFileInput);
	assert( task != NULL );
	task->Go();

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

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXPSPrintSetupDialog::UpdateDisplay()
{
	itsPrintButton->SetActive(JI2B(
		itsDestination->GetSelectedItem() == kPrintToPrinterID ||
		!itsFileInput->IsEmpty()));
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
	const JString& origFullName
	)
{
	if (origFullName.IsEmpty())
		{
		(JGetUserNotification())->ReportError(
			"Please specify a destination file.");
		return kJFalse;
		}

	JString s, path, fileName;
	JSplitPathAndName(origFullName, &s, &fileName);
	if (!JConvertToAbsolutePath(s, NULL, &path) || !JDirectoryExists(path))
		{
		(JGetUserNotification())->ReportError(
			"The specified directory does not exist.");
		return kJFalse;
		}

	const JString fullName    = JCombinePathAndName(path, fileName);
	const JBoolean fileExists = JFileExists(fullName);
	if (!fileExists && !JDirectoryWritable(path))
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
			dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message);
		assert( selection != NULL );
		SetDestination(selection->GetID());
		}

	else if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
		{
		ChooseDestinationFile();
		}
	else if (sender == itsFileInput &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateDisplay();
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

		UpdateDisplay();
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
			GetWindow()->KillFocus();		// values become irrelevant
			GetWindow()->SwitchFocusToFirstWidget();
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
	itsFileInput->SaveFile("Save PostScript file as:");
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

	JString fullName;
	itsFileInput->GetFile(&fullName);

	p->SetDestination(newDest, itsPrintCmd->GetText(), fullName);

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
