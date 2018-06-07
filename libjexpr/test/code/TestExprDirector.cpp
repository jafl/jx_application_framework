/******************************************************************************
 TestExprDirector.cpp

	Main class to test the parsing system and JXExprEditor.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

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

	JXWindow* window = jnew JXWindow(this, 150,150, JString::empty);
	assert( window != nullptr );

	itsTestFEditButton =
		jnew JXTextButton(JGetString("itsTestFEditButton::TestExprDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 150,30);
	assert( itsTestFEditButton != nullptr );

	itsTestDParseButton =
		jnew JXTextButton(JGetString("itsTestDParseButton::TestExprDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,60, 150,30);
	assert( itsTestDParseButton != nullptr );

	itsTestFEqButton =
		jnew JXTextButton(JGetString("itsTestFEqButton::TestExprDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,90, 150,30);
	assert( itsTestFEqButton != nullptr );

	itsTestDEqButton =
		jnew JXTextButton(JGetString("itsTestDEqButton::TestExprDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,120, 150,30);
	assert( itsTestDEqButton != nullptr );

	itsTestCFEditButton =
		jnew JXTextButton(JGetString("itsTestCFEditButton::TestExprDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,30, 150,30);
	assert( itsTestCFEditButton != nullptr );

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
	if ((JGetChooseSaveFile())->ChooseFile("Input file:", nullptr, &fileName))
		{
		TestFEditDirector* dir =
			jnew TestFEditDirector(this, fileName, useComplex);
		assert( dir != nullptr );
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
	if (!(JGetChooseSaveFile())->ChooseFile("Input file:", nullptr, &fileName))
		{
		return;
		}

	std::ifstream input(fileName);
	TestVarList theVarList(input);
	while (1)
		{
		JBoolean expectedResult;
		input >> expectedResult;
		if (input.eof() || input.fail())
			{
			break;
			}

		JDecision* theDecision = nullptr;
		if (!GetDecision(input, &theVarList, &theDecision))
			{
			break;
			}
		else if (theDecision != nullptr)
			{
			theDecision->Print(std::cout);
			std::cout << std::endl;
			std::cout << "Expected: " << expectedResult << std::endl;
			std::cout << "Result:   " << theDecision->Evaluate() << std::endl;
			JWaitForReturn();
			jdelete theDecision;
			}

		input >> std::ws;
		}
}
