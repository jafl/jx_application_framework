/******************************************************************************
 JXPTPageSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXDialogDirector

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPTPageSetupDialog.h>
#include <JXPTPrinter.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jProcessUtil.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPTPageSetupDialog*
JXPTPageSetupDialog::Create
	(
	const JCharacter*	printCmd,
	const JSize			pageWidth,
	const JSize			pageHeight,
	const JSize			minPageHeight,
	const JBoolean		printReverseOrder
	)
{
	JXPTPageSetupDialog* dlog = new JXPTPageSetupDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(printCmd, pageWidth, pageHeight, minPageHeight,
					  printReverseOrder);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPTPageSetupDialog::JXPTPageSetupDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
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
	const JCharacter*	printCmd,
	const JSize			pageWidth,
	const JSize			pageHeight,
	const JSize			minPageHeight,
	const JBoolean		printReverseOrder
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,190, "");
    assert( window != NULL );
    SetWindow(window);

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,19);
    assert( obj1_JXLayout != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 219,159, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXPTPageSetupDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,160, 70,20);
    assert( cancelButton != NULL );

    itsPrintCmd =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 240,20);
    assert( itsPrintCmd != NULL );

    itsWidth =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 90,60, 40,20);
    assert( itsWidth != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 80,20);
    assert( obj2_JXLayout != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 80,20);
    assert( obj3_JXLayout != NULL );

    itsHeight =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 90,80, 40,20);
    assert( itsHeight != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 130,60, 70,20);
    assert( obj4_JXLayout != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 130,80, 70,20);
    assert( obj5_JXLayout != NULL );

    itsPrintTestButton =
        new JXTextButton(JGetString("itsPrintTestButton::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 220,70, 110,20);
    assert( itsPrintTestButton != NULL );

    itsPrintReverseOrderCB =
        new JXTextCheckbox(JGetString("itsPrintReverseOrderCB::JXPTPageSetupDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,120, 190,20);
    assert( itsPrintReverseOrderCB != NULL );
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
	JXTextButton*		okButton,
	JXTextButton*		cancelButton,
	JXInputField*		printCmdInput,
	const JCharacter*	printCmd,
	JXIntegerInput*		widthInput,
	const JSize			pageWidth,
	JXIntegerInput*		heightInput,
	const JSize			pageHeight,
	const JSize			minPageHeight,
	JXTextButton*		printTestButton,
	JXTextCheckbox*		printReverseOrderCB,
	const JBoolean		printReverseOrder
	)
{
	itsPrintCmd            = printCmdInput;
	itsWidth               = widthInput;
	itsHeight              = heightInput;
	itsPrintTestButton     = printTestButton;
	itsPrintReverseOrderCB = printReverseOrderCB;

	(okButton->GetWindow())->SetTitle("Page Setup");
	SetButtons(okButton, cancelButton);

	itsPrintCmd->SetText(printCmd);
	itsPrintCmd->SetIsRequired();
	itsPrintCmd->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

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

JBoolean
JXPTPageSetupDialog::SetParameters
	(
	JXPTPrinter* p
	)
	const
{
	const JString& printCmd = itsPrintCmd->GetText();

	JInteger w,h;
	JBoolean ok = itsWidth->GetValue(&w);
	assert( ok );
	ok = itsHeight->GetValue(&h);
	assert( ok );

	const JBoolean printReverseOrder = itsPrintReverseOrderCB->IsChecked();

	const JBoolean changed =
		JI2B( printCmd          != p->GetPrintCmd()   ||
			  JSize(w)          != p->GetPageWidth()  ||
			  JSize(h)          != p->GetPageHeight() ||
			  printReverseOrder != p->WillPrintReverseOrder() );

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
			ofstream output(fileName);
			for (i=1; i<=w; i++)
				{
				output << JCharacter('0' + i%10);
				}
			output << '\n';
			for (i=2; i<=h; i++)
				{
				output << i << '\n';
				}
			output.close();

			const JString sysCmd  = itsPrintCmd->GetText() + " " + JPrepArgForExec(fileName);
			err = JExecute(sysCmd, NULL);
			err.ReportIfError();

			JRemoveFile(fileName);
			}
		}
}
