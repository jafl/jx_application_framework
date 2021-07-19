/******************************************************************************
 JConjugate.cpp

	This class returns the complex conjugate of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include <JConjugate.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JRect.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JConjugate::JConjugate
	(
	JFunction* arg
	)
	:
	JUnaryFunction("conjugate", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JConjugate::~JConjugate()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JConjugate::JConjugate
	(
	const JConjugate& source
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
JConjugate::Copy()
	const
{
	auto* newFunction = jnew JConjugate(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JConjugate::Evaluate
	(
	JFloat* result
	)
	const
{
	return GetArg()->Evaluate(result);
}

bool
JConjugate::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		jclear_errno();
		*result = conj(value);
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
JConjugate::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	const JSize barHeight = renderer.GetHorizBarHeight();

	// get rectangle for our argument

	JPoint argUpperLeft = upperLeft;
	argUpperLeft.y += barHeight;

	JFunction* arg = GetArg();
	const JIndex argIndex =
		arg->Layout(renderer, argUpperLeft, fontSize, rectList);

	// calculate our rectangle

	JRect ourRect  = rectList->GetRect(argIndex);
	ourRect.top   -= barHeight;

	// save our rectangle

	const JCoordinate ourMidline = rectList->GetMidline(argIndex);
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JConjugate::Render
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

	renderer.DrawHorizBar(ourRect.left, ourRect.top, ourRect.width());
	GetArg()->Render(renderer, rectList);
}
