/******************************************************************************
 TestFEditDirector.cpp

	Class to test JXExprEditor.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "TestFEditDirector.h"
#include "TestVarList.h"
#include "test.expr.fns.h"

#include <JXWindow.h>
#include <JXExprEditorSet.h>
#include <JXExprEditor.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXDeleteObjectTask.h>

#include <JFunction.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFEditDirector::TestFEditDirector
	(
	JXDirector*			supervisor,
	const JCharacter*	fileName,
	const JBoolean		useComplex
	)
	:
	JXWindowDirector(supervisor)
{
	itsInput = jnew std::ifstream(fileName);
	assert( itsInput != nullptr );

	itsVarList = jnew TestVarList(*itsInput);
	assert( itsVarList != nullptr );

	itsUseComplexFlag = useComplex;

	BuildWindow(itsVarList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFEditDirector::~TestFEditDirector()
{
	JXDeleteObjectTask<JBroadcaster>::Delete(itsVarList);

	jdelete itsInput;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestFEditDirector::BuildWindow
	(
	const JVariableList* varList
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,290, JString::empty);
	assert( window != nullptr );

	JXExprEditorSet* exprEditorSet =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,260);
	assert( exprEditorSet != nullptr );

	itsDoneButton =
		jnew JXTextButton(JGetString("itsDoneButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 50,30);
	assert( itsDoneButton != nullptr );

	itsNextButton =
		jnew JXTextButton(JGetString("itsNextButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,0, 50,30);
	assert( itsNextButton != nullptr );

	itsEvalButton =
		jnew JXTextButton(JGetString("itsEvalButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,0, 70,30);
	assert( itsEvalButton != nullptr );

	JXInputField* inputField =
		jnew JXInputField(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,0, 260,20);
	assert( inputField != nullptr );

// end JXLayout

	window->SetTitle("Rendered expression");
	window->SetMinSize(150,150);

	itsExprWidget->Focus();

	ListenTo(itsDoneButton);
	ListenTo(itsNextButton);
	ListenTo(itsEvalButton);
}

/******************************************************************************
 Receive (protected)

	Listen for pushed buttons.

 ******************************************************************************/

void
TestFEditDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDoneButton && message.Is(JXButton::kPushed))
		{
		Close();
		}
	else if (sender == itsNextButton && message.Is(JXButton::kPushed))
		{
		GetNextFunction();
		}
	else if (sender == itsEvalButton && message.Is(JXButton::kPushed))
		{
		EvaluateFunction();
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetNextFunction (private)

 ******************************************************************************/

void
TestFEditDirector::GetNextFunction()
{
	*itsInput >> std::ws;
	if (itsInput->eof() || itsInput->fail())
		{
		Close();
		return;
		}

	JBoolean expectedOK;
	JFunction* theFunction = nullptr;

	if (itsUseComplexFlag)
		{
		JComplex expectedResult;
		while (theFunction == nullptr)
			{
			*itsInput >> expectedOK;
			*itsInput >> expectedResult;
			if (!GetFunction(*itsInput, itsVarList, &theFunction))
				{
				Close();
				return;
				}
			}

		theFunction->Print(std::cout);
		std::cout << std::endl;

		JComplex result;
		const JBoolean ok = theFunction->Evaluate(&result);
		std::cout << "Expected: " << expectedOK << ' ' << expectedResult << std::endl;
		std::cout << "Result:   " << ok << ' ' << result << std::endl;
		}
	else
		{
		JFloat expectedResult;
		while (theFunction == nullptr)
			{
			*itsInput >> expectedOK;
			*itsInput >> expectedResult;
			if (!GetFunction(*itsInput, itsVarList, &theFunction))
				{
				Close();
				return;
				}
			}

		theFunction->Print(std::cout);
		std::cout << std::endl;

		JFloat result;
		const JBoolean ok = theFunction->Evaluate(&result);
		std::cout << "Expected: " << expectedOK << ' ' << expectedResult << std::endl;
		std::cout << "Result:   " << ok << ' ' << result << std::endl;
		}

	itsExprWidget->SetFunction(itsVarList, theFunction);
}

/******************************************************************************
 EvaluateFunction (private)

 ******************************************************************************/

void
TestFEditDirector::EvaluateFunction()
{
	if (itsExprWidget->EndEditing())
		{
		const JFunction* f = itsExprWidget->GetFunction();
		f->Print(std::cout);
		std::cout << std::endl;

		if (itsUseComplexFlag)
			{
			JComplex result;
			std::cout << f->Evaluate(&result) << ' ';
			std::cout << result << std::endl;
			}
		else
			{
			JFloat result;
			std::cout << f->Evaluate(&result) << ' ';
			std::cout << result << std::endl;
			}
		}
}
