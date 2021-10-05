/******************************************************************************
 JXPTPageSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXPTPageSetupDialog.h"
#include "jx-af/jx/JXPTPrinter.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextButton.h"
#include "jx-af/jx/JXTextCheckbox.h"
#include "jx-af/jx/JXIntegerInput.h"
#include "jx-af/jx/JXStaticText.h"
#include "jx-af/jx/JXChooseSaveFile.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPTPageSetupDialog*
JXPTPageSetupDialog::Create
	(
	const JString&	printCmd,
	const JSize		pageWidth,
	const JSize		pageHeight,
	const JSize		minPageHeight,
	const bool	printReverseOrder
	)
{
	auto* dlog = jnew JXPTPageSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(printCmd, pageWidth, pageHeight, minPageHeight,
					  printReverseOrder);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPTPageSetupDialog::JXPTPageSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPTPageSetupDialog::~JXPTPageSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPTPageSetupDialog::BuildWindow
	(
	const JString&	printCmd,
	const JSize		pageWidth,
	const JSize		pageHeight,
	const JSize		minPageHeight,
	const bool	printReverseOrder
	)
{
// begin JXLayout

	const auto* window = jnew JXWindow(this, 370,190, JString::empty);
	assert( window != nullptr );

	const auto* printCmdLabel =
		jnew JXStaticText(JGetString("printCmdLabel::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,19);
	assert( printCmdLabel != nullptr );
	printCmdLabel->SetToLabel();

	const auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXPTPageSetupDialog::shortcuts::JXLayout"));

	const auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,160, 70,20);
	assert( cancelButton != nullptr );

	itsPrintCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 240,20);
	assert( itsPrintCmd != nullptr );

	itsWidth =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,60, 40,20);
	assert( itsWidth != nullptr );

	const auto* widthLabel =
		jnew JXStaticText(JGetString("widthLabel::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 80,20);
	assert( widthLabel != nullptr );
	widthLabel->SetToLabel();

	const auto* heightLabel =
		jnew JXStaticText(JGetString("heightLabel::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 80,20);
	assert( heightLabel != nullptr );
	heightLabel->SetToLabel();

	itsHeight =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,80, 40,20);
	assert( itsHeight != nullptr );

	const auto* charsLabel =
		jnew JXStaticText(JGetString("charsLabel::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,60, 70,20);
	assert( charsLabel != nullptr );
	charsLabel->SetToLabel();

	const auto* linesLabel =
		jnew JXStaticText(JGetString("linesLabel::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,80, 70,20);
	assert( linesLabel != nullptr );
	linesLabel->SetToLabel();

	itsPrintTestButton =
		jnew JXTextButton(JGetString("itsPrintTestButton::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,70, 110,20);
	assert( itsPrintTestButton != nullptr );

	itsPrintReverseOrderCB =
		jnew JXTextCheckbox(JGetString("itsPrintReverseOrderCB::JXPTPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,120, 190,20);
	assert( itsPrintReverseOrderCB != nullptr );
	itsPrintReverseOrderCB->SetShortcuts(JGetString("itsPrintReverseOrderCB::JXPTPageSetupDialog::shortcuts::JXLayout"));

// end JXLayout

	SetObjects(okButton, cancelButton, itsPrintCmd, printCmd,
			   itsWidth, pageWidth, itsHeight, pageHeight, minPageHeight,
			   itsPrintTestButton, itsPrintReverseOrderCB, printReverseOrder);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXPTPageSetupDialog::SetObjects
	(
	JXTextButton*	okButton,
	JXTextButton*	cancelButton,
	JXInputField*	printCmdInput,
	const JString&	printCmd,
	JXIntegerInput*	widthInput,
	const JSize		pageWidth,
	JXIntegerInput*	heightInput,
	const JSize		pageHeight,
	const JSize		minPageHeight,
	JXTextButton*	printTestButton,
	JXTextCheckbox*	printReverseOrderCB,
	const bool	printReverseOrder
	)
{
	itsPrintCmd            = printCmdInput;
	itsWidth               = widthInput;
	itsHeight              = heightInput;
	itsPrintTestButton     = printTestButton;
	itsPrintReverseOrderCB = printReverseOrderCB;

	(okButton->GetWindow())->SetTitle(JGetString("WindowTitle::JXPTPageSetupDialog"));
	SetButtons(okButton, cancelButton);

	itsPrintCmd->GetText()->SetText(printCmd);
	itsPrintCmd->SetIsRequired();
	itsPrintCmd->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsWidth->SetValue(pageWidth);
	itsWidth->SetLowerLimit(1);

	itsHeight->SetValue(pageHeight);
	itsHeight->SetLowerLimit(minPageHeight);

	itsPrintReverseOrderCB->SetState(printReverseOrder);

	ListenTo(itsPrintTestButton);
}

/******************************************************************************
 SetParameters

 ******************************************************************************/

bool
JXPTPageSetupDialog::SetParameters
	(
	JXPTPrinter* p
	)
	const
{
	const JString& printCmd = itsPrintCmd->GetText()->GetText();

	JInteger w,h;
	bool ok = itsWidth->GetValue(&w);
	assert( ok );
	ok = itsHeight->GetValue(&h);
	assert( ok );

	const bool printReverseOrder = itsPrintReverseOrderCB->IsChecked();

	const bool changed =
		printCmd          != p->GetPrintCmd()   ||
			  JSize(w)          != p->GetPageWidth()  ||
			  JSize(h)          != p->GetPageHeight() ||
			  printReverseOrder != p->WillPrintReverseOrder();

	p->SetPrintCmd(printCmd);
	p->SetPageWidth(w);
	p->SetPageHeight(h);
	p->ShouldPrintReverseOrder(printReverseOrder);
	return changed;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXPTPageSetupDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrintTestButton && message.Is(JXButton::kPushed))
	{
		PrintTestPage();
	}

	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 PrintTestPage (private)

 ******************************************************************************/

void
JXPTPageSetupDialog::PrintTestPage()
{
	JInteger i,w,h;
	if (itsPrintCmd->InputValid() &&
		itsWidth->GetValue(&w) && itsHeight->GetValue(&h))
	{
		JString fileName;
		JError err = JCreateTempFile(&fileName);
		if (!err.OK())
		{
			err.ReportIfError();
		}
		else
		{
			std::ofstream output(fileName.GetBytes());
			for (i=1; i<=w; i++)
			{
				output << JUtf8Byte('0' + i%10);
			}
			output << '\n';
			for (i=2; i<=h; i++)
			{
				output << i << '\n';
			}
			output.close();

			const JString sysCmd  = itsPrintCmd->GetText()->GetText() + JString(" ", JString::kNoCopy) + JPrepArgForExec(fileName);
			err = JExecute(sysCmd, nullptr);
			err.ReportIfError();

			JRemoveFile(fileName);
		}
	}
}
