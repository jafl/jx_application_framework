/******************************************************************************
 JConjugate.cpp

	This class returns the complex conjugate of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal. All rights reserved.

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

JConjugate::JConjugate()
	:
	JUnaryFunction(kJConjugateNameIndex, kJConjugateType)
{
}

JConjugate::JConjugate
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJConjugateNameIndex, kJConjugateType)
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
	JConjugate* newFunction = new JConjugate(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JConjugate::Evaluate
	(
	JFloat* result
	)
	const
{
	return GetArg()->Evaluate(result);
}

JBoolean
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
		return kJFalse;
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JConjugate::PrepareToRender
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
		arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);

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
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);

	// draw ourselves

	renderer.DrawHorizBar(ourRect.left, ourRect.top, ourRect.width());
	GetArg()->Render(renderer, rectList);
}
