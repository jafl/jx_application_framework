/******************************************************************************
 JProduct.cpp

								The Product Class

	This class returns the product of its arguments.

	BASE CLASS = JNaryOperator

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JProduct.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JProduct::JProduct()
	:
	JNaryOperator(kJMultiplicationNameIndex, kJProductType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JProduct::~JProduct()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JProduct::JProduct
	(
	const JProduct& source
	)
	:
	JNaryOperator(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JProduct::Copy()
	const
{
	JProduct* newFunction = jnew JProduct(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JProduct::Evaluate
	(
	JFloat* result
	)
	const
{
	const JSize argCount = GetArgCount();
	if (argCount == 0)
		{
		return kJFalse;
		}

	*result = 1.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		*result *= argValue;
		}

	return kJTrue;
}

JBoolean
JProduct::Evaluate
	(
	JComplex* result
	)
	const
{
	const JSize argCount = GetArgCount();
	if (argCount == 0)
		{
		return kJFalse;
		}

	*result = 1.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JComplex argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		*result *= argValue;
		}

	return kJTrue;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JProduct::Print
	(
	ostream& output
	)
	const
{
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		if (i > 1)
			{
			output << JPGetMultiplicationString();
			}

		const JFunction* arg = GetArg(i);
		if (ParenthesizeArgForPrint(*this, *arg))
			{
			output << '(';
			arg->Print(output);
			output << ')';
			}
		else
			{
			arg->Print(output);
			}
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JProduct::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle and midline

	JPoint argUpperLeft = upperLeft;

	JRect ourRect(upperLeft, argUpperLeft);
	ourRect.bottom += renderer.GetLineHeight(fontSize);
	JCoordinate ourMidline = (ourRect.top + ourRect.bottom)/2;
	const JCoordinate origMidline = ourMidline;

	// get rectangle for each argument

	const JSize spaceWidth = renderer.GetStringWidth(fontSize, " ");
	const JSize timesWidth = renderer.GetStringWidth(fontSize, renderer.GetMultiplicationString());
	const JSize argCount   = GetArgCount();
	{
	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* arg = GetArg(i);
		const JIndex argIndex =
			arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
		JRect argRect  = rectList->GetRect(argIndex);
		argUpperLeft.x = argRect.right + spaceWidth + timesWidth;

		if (ParenthesizeArgForRender(*this, *arg))
			{
			const JSize parenWidth = renderer.GetParenthesisWidth(argRect.height());
			rectList->ShiftRect(argIndex, parenWidth, 0);
			argRect = rectList->GetRect(argIndex);
			argUpperLeft.x += 2*parenWidth;
			ourRect.right   = argRect.right + parenWidth;
			}

		ourRect = JCovering(ourRect, argRect);
		const JCoordinate argMidline = rectList->GetMidline(argIndex);
		if (argMidline > ourMidline)
			{
			ourMidline = argMidline;
			}
		}
	}

	// adjust the argument rectangles so all the midlines are the same
	// (ourMidline is guaranteed to stay constant)

	if (argCount > 1 && ourMidline > origMidline)
		{
		for (JIndex i=1; i<=argCount; i++)
			{
			const JFunction* arg = GetArg(i);
			JIndex argIndex;
			const JBoolean found = rectList->FindFunction(arg, &argIndex);
			assert( found );
			rectList->SetMidline(argIndex, ourMidline);
			ourRect = JCovering(ourRect, rectList->GetRect(argIndex));
			}
		}

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JProduct::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize         = rectList.GetFontSize(ourIndex);

	// draw ourselves

	const JCharacter* timesStr = renderer.GetMultiplicationString();

	const JSize spaceWidth = renderer.GetStringWidth(fontSize, " ")/2;
	const JSize timesWidth = renderer.GetStringWidth(fontSize, timesStr);

	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		const JFunction* arg = GetArg(i);
		arg->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		const JRect argRect = rectList.GetRect(argIndex);
		JCoordinate h       = argRect.right + spaceWidth;

		if (ParenthesizeArgForRender(*this, *arg))
			{
			renderer.DrawParentheses(argRect);
			h += renderer.GetParenthesisWidth(argRect.height());
			}

		if (i < argCount)
			{
			renderer.DrawString(h, ourMidline, fontSize, timesStr);
			}
		}
}
