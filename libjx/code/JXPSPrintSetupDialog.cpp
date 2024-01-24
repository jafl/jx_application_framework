/******************************************************************************
 JXPSPrintSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXPSPrintSetupDialog.h"
#include "JXAdjustPrintSetupLayoutTask.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXIntegerInput.h"
#include "JXFileInput.h"
#include "JXTextCheckbox.h"
#include "JXRadioGroup.h"
#include "JXTextRadioButton.h"
#include "JXFileNameDisplay.h"
#include "JXCSFDialogBase.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

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
	const JString&					printCmd,
	const JString&					fileName,
	const bool						collate,
	const bool						bw
	)
{
	auto* dlog = jnew JXPSPrintSetupDialog;
	dlog->BuildWindow(dest, printCmd, fileName, collate, bw);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPSPrintSetupDialog::JXPSPrintSetupDialog()
	:
	JXModalDialogDirector()
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
	const JString&					printCmd,
	const JString&					fileName,
	const bool						collate,
	const bool						bw
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,250, JGetString("WindowTitle::JXPSPrintSetupDialog::JXLayout"));

	itsDestination =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 139,39);

	auto* destinationLabel =
		jnew JXStaticText(JGetString("destinationLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,30, 80,20);
	destinationLabel->SetToLabel(false);

	auto* printerLabel =
		jnew JXTextRadioButton(1, JGetString("printerLabel::JXPSPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,8, 70,20);
	printerLabel->SetShortcuts(JGetString("printerLabel::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	auto* fileLabel =
		jnew JXTextRadioButton(2, JGetString("fileLabel::JXPSPrintSetupDialog::JXLayout"), itsDestination,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 75,8, 50,20);
	fileLabel->SetShortcuts(JGetString("fileLabel::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	itsPrintCmdLabel =
		jnew JXStaticText(JGetString("itsPrintCmdLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 90,20);
	itsPrintCmdLabel->SetToLabel(false);

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 80,20);
	itsChooseFileButton->SetShortcuts(JGetString("itsChooseFileButton::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 55,110, 115,20);
	countLabel->SetToLabel(false);

	itsCollateCB =
		jnew JXTextCheckbox(JGetString("itsCollateCB::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,110, 70,20);
	itsCollateCB->SetShortcuts(JGetString("itsCollateCB::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	itsPrintAllCB =
		jnew JXTextCheckbox(JGetString("itsPrintAllCB::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,150, 110,20);
	itsPrintAllCB->SetShortcuts(JGetString("itsPrintAllCB::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	itsFirstPageIndexLabel =
		jnew JXStaticText(JGetString("itsFirstPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,150, 70,20);
	itsFirstPageIndexLabel->SetToLabel(false);

	itsLastPageIndexLabel =
		jnew JXStaticText(JGetString("itsLastPageIndexLabel::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 270,150, 20,20);
	itsLastPageIndexLabel->SetToLabel(false);

	itsBWCheckbox =
		jnew JXTextCheckbox(JGetString("itsBWCheckbox::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,190, 150,20);
	itsBWCheckbox->SetShortcuts(JGetString("itsBWCheckbox::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,220, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXPSPrintSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 219,219, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXPSPrintSetupDialog::JXLayout"));

	itsPrintCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,70, 250,20);

	itsCopyCount =
		jnew JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,110, 40,20);
	itsCopyCount->SetIsRequired(false);

	itsFirstPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,150, 40,20);
	itsFirstPageIndex->SetIsRequired(false);

	itsLastPageIndex =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 290,150, 40,20);
	itsLastPageIndex->SetIsRequired(false);

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
	const JString&					printCmd,
	JXTextButton*					chooseFileButton,
	const JString&					fileName,
	JXIntegerInput*					copyCount,
	JXTextCheckbox*					collateCheckbox,
	const bool						collate,
	JXTextCheckbox*					bwCheckbox,
	const bool						bw,
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

	SetButtons(okButton, cancelButton);

	const JRect r1 = itsPrintCmd->GetFrame(),
				r2 = itsChooseFileButton->GetFrame();
	itsFileInput =
		jnew JXFileInput(GetWindow(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 r1.left, r2.top, r1.width(), r2.height());
	assert( itsFileInput != nullptr );
	itsFileInput->ShouldAllowInvalidFile();
	itsFileInput->GetText()->SetText(fileName);
	ListenTo(itsFileInput);

	auto* task =
		jnew JXAdjustPrintSetupLayoutTask(this, itsPrintCmd, itsChooseFileButton, itsFileInput);
	task->Go();

	itsPrintCmd->GetText()->SetText(printCmd);
	itsPrintCmd->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	ListenTo(itsDestination);
	ListenTo(itsChooseFileButton);
	ListenTo(itsPrintAllCB);

	bool foundDest = false;
	for (JIndex i=1; i<=kDestCount; i++)
	{
		if (kIndexToDest[i-1] == dest)
		{
			SetDestination(i);
			foundDest = true;
			break;
		}
	}
	assert( foundDest );

	itsCopyCount->SetValue(1);
	itsCopyCount->SetLowerLimit(1);

	PrintAllPages(true);

	itsFirstPageIndex->SetLowerLimit(1);
	itsFirstPageIndex->SetIsRequired(false);

	itsLastPageIndex->SetLowerLimit(1);
	itsLastPageIndex->SetIsRequired(false);

	itsCollateCB->SetState(collate);
	itsBWCheckbox->SetState(bw);

	itsChooseFileButton->SetShortcuts(JGetString("ChooseFileShortcut::JXPSPrintSetupDialog"));
	itsBWCheckbox->SetShortcuts(JGetString("BWCheckboxShortcut::JXPSPrintSetupDialog"));
	itsPrintAllCB->SetShortcuts(JGetString("PrintAllShortcut::JXPSPrintSetupDialog"));
	itsCollateCB->SetShortcuts(JGetString("CollateCBShortcut::JXPSPrintSetupDialog"));

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
JXPSPrintSetupDialog::UpdateDisplay()
{
	itsPrintButton->SetActive(itsDestination->GetSelectedItem() == kPrintToPrinterID ||
		!itsFileInput->GetText()->IsEmpty());
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXPSPrintSetupDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	if (itsDestination->GetSelectedItem() == kPrintToFileID)
	{
		return OKToDeactivate(itsFileInput->GetText()->GetText());
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 OKToDeactivate (static)

 ******************************************************************************/

bool
JXPSPrintSetupDialog::OKToDeactivate
	(
	const JString& origFullName
	)
{
	if (origFullName.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("MissingFileName::JXPSPrintSetupDialog"));
		return false;
	}

	JString s, path, fileName;
	JSplitPathAndName(origFullName, &s, &fileName);
	if (!JConvertToAbsolutePath(s, JString::empty, &path) || !JDirectoryExists(path))
	{
		JGetUserNotification()->ReportError(JGetString("DirectoryDoesNotExist::JXGlobal"));
		return false;
	}

	const JString fullName    = JCombinePathAndName(path, fileName);
	const bool fileExists = JFileExists(fullName);
	if (!fileExists && !JDirectoryWritable(path))
	{
		JGetUserNotification()->ReportError(JGetString("DirNotWritable::JXPSPrintSetupDialog"));
		return false;
	}
	else if (fileExists && !JFileWritable(fullName))
	{
		JGetUserNotification()->ReportError(JGetString("FileNotWritable::JXGlobal"));
		return false;
	}

	return true;
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
		const auto* selection =
			dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message);
		assert( selection != nullptr );
		SetDestination(selection->GetID());
	}

	else if (sender == itsChooseFileButton && message.Is(JXButton::kPushed))
	{
		ChooseDestinationFile();
	}
	else if (sender == itsFileInput &&
			 (message.Is(JStyledText::kTextSet) ||
			  message.Is(JStyledText::kTextChanged)))
	{
		UpdateDisplay();
	}

	else if (sender == itsPrintAllCB && message.Is(JXCheckbox::kPushed))
	{
		PrintAllPages(itsPrintAllCB->IsChecked());
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
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
		itsPrintCmd->SetIsRequired(true);
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
		itsPrintCmd->SetIsRequired(false);
		itsCollateCB->Hide();
		itsChooseFileButton->Show();
		itsFileInput->Show();
		itsFileInput->Focus();

		UpdateDisplay();
		if (itsFileInput->GetText()->IsEmpty())
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
	const bool all
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
	itsFileInput->SaveFile(JGetString("SaveFilePrompt::JXPSPrintSetupDialog"));
}

/******************************************************************************
 SetParameters (virtual)

	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
JXPSPrintSetupDialog::SetParameters
	(
	JXPSPrinter* p
	)
	const
{
	const JXPSPrinter::Destination newDest =
		kIndexToDest[ itsDestination->GetSelectedItem()-1 ];

	bool changed = newDest                            != p->GetDestination() ||
		itsPrintCmd->GetText()->GetText()  != p->GetPrintCmd()    ||
		itsFileInput->GetText()->GetText() != p->GetFileName();

	JString fullName;
	itsFileInput->GetFile(&fullName);

	p->SetDestination(newDest, itsPrintCmd->GetText()->GetText(), fullName);

	JInteger copyCount;
	const bool ok = itsCopyCount->GetValue(&copyCount);
	assert( ok );
	p->SetCopyCount(copyCount);

	const bool printAll = itsPrintAllCB->IsChecked();
	p->PrintAllPages();

	if (!printAll)
	{
		JInteger p1, p2;
		const bool ok1 = itsFirstPageIndex->GetValue(&p1);
		const bool ok2 = itsLastPageIndex->GetValue(&p2);
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

	changed = changed ||
		itsCollateCB->IsChecked()  != p->WillCollatePages() ||
		itsBWCheckbox->IsChecked() != p->WillPrintBlackWhite();

	p->CollatePages(itsCollateCB->IsChecked());
	p->PSPrintBlackWhite(itsBWCheckbox->IsChecked());

	return changed;
}
