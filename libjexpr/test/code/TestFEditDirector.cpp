/******************************************************************************
 TestFEditDirector.cpp

	Class to test JXExprEditor.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996 by John Lindal. All rights reserved.

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
	itsInput = jnew ifstream(fileName);
	assert( itsInput != NULL );

	itsVarList = jnew TestVarList(*itsInput);
	assert( itsVarList != NULL );

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

	JXWindow* window = jnew JXWindow(this, 500,290, "");
	assert( window != NULL );

	JXExprEditorSet* obj1_JXLayout =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,260);
	assert( obj1_JXLayout != NULL );

	itsDoneButton =
		jnew JXTextButton(JGetString("itsDoneButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 50,30);
	assert( itsDoneButton != NULL );

	itsNextButton =
		jnew JXTextButton(JGetString("itsNextButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,0, 50,30);
	assert( itsNextButton != NULL );

	itsEvalButton =
		jnew JXTextButton(JGetString("itsEvalButton::TestFEditDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,0, 70,30);
	assert( itsEvalButton != NULL );

	JXInputField* obj2_JXLayout =
		jnew JXInputField(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,0, 260,20);
	assert( obj2_JXLayout != NULL );

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
	*itsInput >> ws;
	if (itsInput->eof() || itsInput->fail())
		{
		Close();
		return;
		}

	JBoolean expectedOK;
	JFunction* theFunction = NULL;

	if (itsUseComplexFlag)
		{
		JComplex expectedResult;
		while (theFunction == NULL)
			{
			*itsInput >> expectedOK;
			*itsInput >> expectedResult;
			if (!GetFunction(*itsInput, itsVarList, &theFunction))
				{
				Close();
				return;
				}
			}

		theFunction->Print(cout);
		cout << endl;

		JComplex result;
		const JBoolean ok = theFunction->Evaluate(&result);
		cout << "Expected: " << expectedOK << ' ' << expectedResult << endl;
		cout << "Result:   " << ok << ' ' << result << endl;
		}
	else
		{
		JFloat expectedResult;
		while (theFunction == NULL)
			{
			*itsInput >> expectedOK;
			*itsInput >> expectedResult;
			if (!GetFunction(*itsInput, itsVarList, &theFunction))
				{
				Close();
				return;
				}
			}

		theFunction->Print(cout);
		cout << endl;

		JFloat result;
		const JBoolean ok = theFunction->Evaluate(&result);
		cout << "Expected: " << expectedOK << ' ' << expectedResult << endl;
		cout << "Result:   " << ok << ' ' << result << endl;
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
		f->Print(cout);
		cout << endl;

		if (itsUseComplexFlag)
			{
			JComplex result;
			cout << f->Evaluate(&result) << ' ';
			cout << result << endl;
			}
		else
			{
			JFloat result;
			cout << f->Evaluate(&result) << ' ';
			cout << result << endl;
			}
		}
}
