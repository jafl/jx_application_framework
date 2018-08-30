/******************************************************************************
 JAbsValue.cpp

								The Absolute Value Class

	This class returns the absolute value of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JAbsValue.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JRect.h>
#include <math.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JAbsValue::JAbsValue
	(
	JFunction* arg
	)
	:
	JUnaryFunction("abs", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JAbsValue::~JAbsValue()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JAbsValue::JAbsValue
	(
	const JAbsValue& source
	)
	:
	JUnaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JAbsValue::Copy()
	const
{
	JAbsValue* newFunction = jnew JAbsValue(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JAbsValue::Evaluate
	(
	JFloat* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		jclear_errno();
		*result = abs(value);
		return jerrno_is_clear();
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JAbsValue::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		jclear_errno();
		*result = abs(value);
		return jerrno_is_clear();
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JAbsValue::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	const JSize barWidth = renderer.GetVertBarWidth();

	// get rectangle for our argument

	JPoint argUpperLeft = upperLeft;
	argUpperLeft.x += barWidth;

	JFunction* arg = GetArg();
	const JIndex argIndex =
		arg->Layout(renderer, argUpperLeft, fontSize, rectList);

	// calculate our rectangle

	JRect ourRect  = rectList->GetRect(argIndex);
	ourRect.left  -= barWidth;
	ourRect.right += barWidth;

	// save our rectangle

	const JCoordinate ourMidline = rectList->GetMidline(argIndex);
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JAbsValue::Render
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

	const JRect ourRect = rectList.GetRect(ourIndex);

	// draw ourselves

	renderer.DrawVertBar(ourRect.left, ourRect.top, ourRect.height());
	GetArg()->Render(renderer, rectList);
	renderer.DrawVertBar(ourRect.right - renderer.GetVertBarWidth(),
						 ourRect.top, ourRect.height());
}
