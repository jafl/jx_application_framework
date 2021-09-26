/******************************************************************************
 JLogB.cpp

	This class calculates the log of a JFunction to a specified base b > 1.0.

	BASE CLASS = JBinaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JLogB.h"
#include "jx-af/jexpr/JExprRenderer.h"
#include "jx-af/jexpr/JExprRectList.h"
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JLogB::JLogB
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryFunction("log", arg1, arg2)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JLogB::~JLogB()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JLogB::JLogB
	(
	const JLogB& source
	)
	:
	JBinaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JLogB::Copy()
	const
{
	auto* newFunction = jnew JLogB(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JLogB::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat b,x;
	if (!(GetArg1())->Evaluate(&b) || b <= 1.0)
	{
		return false;
	}
	if (!(GetArg2())->Evaluate(&x))
	{
		return false;
	}
	jclear_errno();
	*result = log(x)/log(b);
	return jerrno_is_clear();
}

bool
JLogB::Evaluate
	(
	JComplex* result
	)
	const
{
	JFloat b;
	JComplex x;
	if (!(GetArg1())->Evaluate(&b) || b <= 1.0)
	{
		return false;
	}
	if (!(GetArg2())->Evaluate(&x))
	{
		return false;
	}
	jclear_errno();
	*result = log(x)/log(b);
	return jerrno_is_clear();
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JLogB::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle

	const JString& fnName = GetName();

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = upperLeft.y + renderer.GetLineHeight(fontSize);
	ourRect.right  = upperLeft.x + renderer.GetStringWidth(fontSize, fnName);

	// get rectangle for base

	JPoint argUpperLeft(ourRect.right, ourRect.top);
	const JSize baseFontSize = renderer.GetSuperSubFontSize(fontSize);

	JFunction* base = GetArg1();
	const JIndex baseIndex =
		base->Layout(renderer, argUpperLeft, baseFontSize, rectList);
	const JRect baseRect = rectList->GetRect(baseIndex);
	argUpperLeft.x = baseRect.right;

	// get rectangle for argument -- gives our midline

	JFunction* arg = GetArg2();
	const JIndex argIndex =
		arg->Layout(renderer, argUpperLeft, fontSize, rectList);
	const JRect argRect          = rectList->GetRect(argIndex);
	ourRect                      = JCovering(ourRect, argRect);
	const JCoordinate ourMidline = rectList->GetMidline(argIndex);

	// shift argument to make space for left parenthesis

	const JSize parenWidth = renderer.GetParenthesisWidth(argRect.height());
	rectList->ShiftRect(argIndex, parenWidth, 0);

	// we need space for two parentheses

	ourRect.right += 2*parenWidth;

	// shift the base down

	rectList->ShiftRect(baseIndex, 0, ourMidline - ourRect.top);
	ourRect = JCovering(ourRect, rectList->GetRect(baseIndex));

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JLogB::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	// find ourselves in the list

	JIndex ourIndex;
	const bool found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);
	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize = rectList.GetFontSize(ourIndex);

	// draw ourselves

	renderer.DrawString(ourRect.left, ourMidline, fontSize, GetName());

	// draw the base

	(GetArg1())->Render(renderer, rectList);

	// draw the argument

	const JFunction* arg = GetArg2();
	arg->Render(renderer, rectList);

	JIndex argIndex;
	const bool foundArg = rectList.FindFunction(arg, &argIndex);
	assert( foundArg );
	renderer.DrawParentheses(rectList.GetRect(argIndex));
}
