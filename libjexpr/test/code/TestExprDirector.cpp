/******************************************************************************
 TestExprDirector.cpp

	Main class to test the parsing system and JXExprEditor.

	BASE CLASS = JXWindowDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestExprDirector.h"
#include "TestFEditDirector.h"
#include "TestVarList.h"
#include "test.expr.fns.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <jXGlobals.h>

#include <JDecision.h>
#include <JString.h>
#include <jCommandLine.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestExprDirector::TestExprDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestExprDirector::~TestExprDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestExprDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 150,150, "");
    assert( window != NULL );

    itsTestFEditButton =
        new JXTextButton(JGetString("itsTestFEditButton::TestExprDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 150,30);
    assert( itsTestFEditButton != NULL );

    itsTestDParseButton =
        new JXTextButton(JGetString("itsTestDParseButton::TestExprDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,60, 150,30);
    assert( itsTestDParseButton != NULL );

    itsTestFEqButton =
        new JXTextButton(JGetString("itsTestFEqButton::TestExprDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,90, 150,30);
    assert( itsTestFEqButton != NULL );

    itsTestDEqButton =
        new JXTextButton(JGetString("itsTestDEqButton::TestExprDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,120, 150,30);
    assert( itsTestDEqButton != NULL );

    itsTestCFEditButton =
        new JXTextButton(JGetString("itsTestCFEditButton::TestExprDirector::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,30, 150,30);
    assert( itsTestCFEditButton != NULL );

// end JXLayout

	window->SetTitle("Test Parser");
	window->SetCloseAction(JXWindow::kQuitApp);
	window->LockCurrentSize();

	ListenTo(itsTestFEditButton);
	ListenTo(itsTestCFEditButton);
	ListenTo(itsTestDParseButton);
	ListenTo(itsTestFEqButton);
	ListenTo(itsTestDEqButton);
}

/******************************************************************************
 Receive (protected)

	Listen for pushed buttons.

 ******************************************************************************/

void
TestExprDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTestFEditButton && message.Is(JXButton::kPushed))
		{
		TestFunctionEditor(kJFalse);
		}
	if (sender == itsTestCFEditButton && message.Is(JXButton::kPushed))
		{
		TestFunctionEditor(kJTrue);
		}
	else if (sender == itsTestDParseButton && message.Is(JXButton::kPushed))
		{
		TestDecisionParser();
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 TestFunctionEditor (private)

 ******************************************************************************/

void
TestExprDirector::TestFunctionEditor
	(
	const JBoolean useComplex
	)
{
	JString fileName;
	if ((JGetChooseSaveFile())->ChooseFile("Input file:", NULL, &fileName))
		{
		TestFEditDirector* dir =
			new TestFEditDirector(this, fileName, useComplex);
		assert( dir != NULL );
		dir->Activate();
		}
}

/******************************************************************************
 TestDecisionParser (private)

 ******************************************************************************/

void
TestExprDirector::TestDecisionParser()
{
	JString fileName;
	if (!(JGetChooseSaveFile())->ChooseFile("Input file:", NULL, &fileName))
		{
		return;
		}

	ifstream input(fileName);
	TestVarList theVarList(input);
	while (1)
		{
		JBoolean expectedResult;
		input >> expectedResult;
		if (input.eof() || input.fail())
			{
			break;
			}

		JDecision* theDecision = NULL;
		if (!GetDecision(input, &theVarList, &theDecision))
			{
			break;
			}
		else if (theDecision != NULL)
			{
			theDecision->Print(cout);
			cout << endl;
			cout << "Expected: " << expectedResult << endl;
			cout << "Result:   " << theDecision->Evaluate() << endl;
			JWaitForReturn();
			delete theDecision;
			}

		input >> ws;
		}
}
