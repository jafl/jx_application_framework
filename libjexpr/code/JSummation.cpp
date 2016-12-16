/******************************************************************************
 JSummation.cpp

								The Summation Class

	This class returns the sum of its arguments.

	BASE CLASS = JNaryOperator

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JSummation.h>
#include <JNegation.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JSummation::JSummation()
	:
	JNaryOperator(kJAdditionNameIndex, kJSummationType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSummation::~JSummation()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JSummation::JSummation
	(
	const JSummation& source
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
JSummation::Copy()
	const
{
	JSummation* newFunction = jnew JSummation(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JSummation::Evaluate
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

	*result = 0.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		*result += argValue;
		}

	return kJTrue;
}

JBoolean
JSummation::Evaluate
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

	*result = 0.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JComplex argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		*result += argValue;
		}

	return kJTrue;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JSummation::Print
	(
	std::ostream& output
	)
	const
{
	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		const JFunction* f   = this;
		const JFunction* arg = GetArg(i);
		if (arg->GetType() == kJNegationType)
			{
			output << JPGetSubtractionString();
			f = arg;
			const JNegation* neg = dynamic_cast<const JNegation*>(arg);
			assert( neg != NULL );
			arg = neg->GetArg();
			}
		else if (i > 1)
			{
			output << JPGetAdditionString();
			}

		if (ParenthesizeArgForPrint(*f, *arg))
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
JSummation::PrepareToRender
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
	JCoordinate ourMidline = ourRect.ycenter();
	const JCoordinate origMidline = ourMidline;

	// get rectangle for each argument

	const JSize spaceWidth = renderer.GetStringWidth(fontSize, " ");
	const JSize plusWidth  = renderer.GetStringWidth(fontSize, JPGetAdditionString());
	const JSize minusWidth = renderer.GetStringWidth(fontSize, JPGetSubtractionString());

	const JSize argCount = GetArgCount();
	{
	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* f   = this;
		JFunction* arg = GetArg(i);
		if (arg->GetType() == kJNegationType)
			{
			argUpperLeft.x += minusWidth + spaceWidth;
			if (i > 1)
				{
				argUpperLeft.x += spaceWidth;
				}
			f = arg;
			JNegation* neg = dynamic_cast<JNegation*>(arg);
			assert( neg != NULL );
			arg = neg->GetArg();
			}
		else if (i > 1)
			{
			argUpperLeft.x += plusWidth + 2*spaceWidth;
			}

		const JIndex argIndex =
			arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
		JRect argRect = rectList->GetRect(argIndex);
		argUpperLeft.x = argRect.right;

		if (ParenthesizeArgForRender(*f, *arg))
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
			if (arg->GetType() == kJNegationType)
				{
				const JNegation* neg = dynamic_cast<const JNegation*>(arg);
				assert( neg != NULL );
				arg = neg->GetArg();
				}

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
JSummation::Render
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

	const JRect ourRect          = rectList.GetRect(ourIndex);
	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize         = rectList.GetFontSize(ourIndex);

	// draw ourselves

	JCoordinate h          = ourRect.left;
	const JSize spaceWidth = renderer.GetStringWidth(fontSize, " ");

	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		const JFunction* f   = this;
		const JFunction* arg = GetArg(i);
		if (arg->GetType() == kJNegationType)
			{
			renderer.DrawString(h, ourMidline, fontSize, JPGetSubtractionString());
			f = arg;
			const JNegation* neg = dynamic_cast<const JNegation*>(arg);
			assert( neg != NULL );
			arg = neg->GetArg();
			}
		else if (i > 1)
			{
			renderer.DrawString(h, ourMidline, fontSize, JPGetAdditionString());
			}

		arg->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		const JRect argRect = rectList.GetRect(argIndex);
		h = argRect.right;

		if (ParenthesizeArgForRender(*f, *arg))
			{
			renderer.DrawParentheses(argRect);
			h += renderer.GetParenthesisWidth(argRect.height());
			}

		h += spaceWidth;
		}
}
