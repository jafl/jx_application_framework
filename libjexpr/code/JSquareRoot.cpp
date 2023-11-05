/******************************************************************************
 JSquareRoot.cpp

								The Square Root Class

	This class calculates the square root of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JSquareRoot.h"
#include "JExprRenderer.h"
#include "JExprRectList.h"
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JSquareRoot::JSquareRoot
	(
	JFunction* arg
	)
	:
	JUnaryFunction("sqrt", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSquareRoot::~JSquareRoot()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JSquareRoot::JSquareRoot
	(
	const JSquareRoot& source
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
JSquareRoot::Copy()
	const
{
	auto* newFunction = jnew JSquareRoot(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JSquareRoot::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (GetArg()->Evaluate(&argValue) && argValue >= 0.0)
	{
		jclear_errno();
		*result = sqrt(argValue);
		return jerrno_is_clear();
	}
	else
	{
		return false;
	}
}

bool
JSquareRoot::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (GetArg()->Evaluate(&argValue))
	{
		jclear_errno();
		*result = sqrt(argValue);
		return jerrno_is_clear();
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JSquareRoot::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// get rectangle for our argument

	JFunction* arg = GetArg();
	const JIndex argIndex =
		arg->Layout(renderer, upperLeft, fontSize, rectList);

	// shift our argument to make space for the square root sign

	JRect argRect = rectList->GetRect(argIndex);
	const JSize argHeight = argRect.height();
	rectList->ShiftRect(argIndex, renderer.GetSquareRootLeadingWidth(argHeight),
						renderer.GetSquareRootExtraHeight());
	argRect = rectList->GetRect(argIndex);

	// calculate our rectangle

	JRect ourRect  = argRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.right += renderer.GetSquareRootTrailingWidth(argHeight);

	// save our rectangle

	const JCoordinate ourMidline = rectList->GetMidline(argIndex);
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JSquareRoot::Render
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

	// draw ourselves

	renderer.DrawSquareRoot(ourRect);
	GetArg()->Render(renderer, rectList);
}
