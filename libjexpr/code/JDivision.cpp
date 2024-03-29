/******************************************************************************
 JDivision.cpp

								The JDivision Class

	This class divides two functions.  If the denominator is zero, it returns zero.

	BASE CLASS = JBinaryOperator

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JDivision.h"
#include "JExprRenderer.h"
#include "JExprRectList.h"
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JDivision::JDivision
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryOperator("/", arg1, arg2)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDivision::~JDivision()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDivision::JDivision
	(
	const JDivision& source
	)
	:
	JBinaryOperator(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JDivision::Copy()
	const
{
	auto* newFunction = jnew JDivision(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JDivision::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat numerator, denominator;
	if (!GetArg1()->Evaluate(&numerator))
	{
		return false;
	}
	if (!GetArg2()->Evaluate(&denominator))
	{
		return false;
	}

	if (denominator != 0.0)
	{
		*result = numerator / denominator;
		return true;
	}
	else
	{
		return false;
	}
}

bool
JDivision::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex numerator, denominator;
	if (!GetArg1()->Evaluate(&numerator))
	{
		return false;
	}
	if (!GetArg2()->Evaluate(&denominator))
	{
		return false;
	}

	if (denominator != 0.0)
	{
		*result = numerator / denominator;
		return true;
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
JDivision::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle

	JRect ourRect(upperLeft, upperLeft);
	ourRect.bottom += renderer.GetLineHeight(fontSize);

	// get rectangle for numerator

	const JCoordinate spaceWidth = renderer.GetSpaceWidth(fontSize);
	JPoint argUpperLeft = upperLeft;
	argUpperLeft.x += spaceWidth;

	JFunction* numerator = GetArg1();
	const JIndex numIndex =
		numerator->Layout(renderer, argUpperLeft, fontSize, rectList);
	const JRect numRect = rectList->GetRect(numIndex);
	argUpperLeft.y = numRect.bottom;
	ourRect        = JCovering(ourRect, numRect);

	// create space for division line

	const JCoordinate barHeight = renderer.GetHorizBarHeight();
	const JCoordinate ourMidline = argUpperLeft.y + barHeight/2;
	argUpperLeft.y += barHeight;

	// get rectangle for denominator

	JFunction* denominator = GetArg2();
	const JIndex denIndex =
		denominator->Layout(renderer, argUpperLeft, fontSize, rectList);
	const JRect denRect = rectList->GetRect(denIndex);
	ourRect             = JCovering(ourRect, denRect);

	// align centers of numerator and denominator horizontally
	// (this is guaranteed to leave ourRect constant)

	const JCoordinate deltah = (numRect.right - denRect.right)/2;
	if (deltah > 0)
	{
		rectList->ShiftRect(denIndex, deltah, 0);
	}
	else if (deltah < 0)
	{
		rectList->ShiftRect(numIndex, -deltah, 0);
	}

	// add one extra space at the right so division line is wider than arguments

	ourRect.right += spaceWidth;

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JDivision::Render
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
	const JCoordinate barTop = ourMidline - renderer.GetHorizBarHeight()/2;

	// draw ourselves

	(GetArg1())->Render(renderer, rectList);
	renderer.DrawHorizBar(ourRect.left, barTop, ourRect.width());
	(GetArg2())->Render(renderer, rectList);
}
