/******************************************************************************
 JExponent.cpp

								The JExponent Class

	This class computes (arg1)^arg2.

	BASE CLASS = JBinaryOperator

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JExponent.h"
#include "jx-af/jexpr/JExprRenderer.h"
#include "jx-af/jexpr/JExprRectList.h"
#include "jx-af/jexpr/jFunctionUtil.h"
#include <jx-af/jcore/JRect.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JExponent::JExponent
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryOperator("^", arg1, arg2)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExponent::~JExponent()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JExponent::JExponent
	(
	const JExponent& source
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
JExponent::Copy()
	const
{
	auto* newFunction = jnew JExponent(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JExponent::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value, exponent;
	if (!GetArg1()->Evaluate(&value))
	{
		return false;
	}
	if (!GetArg2()->Evaluate(&exponent))
	{
		return false;
	}
	jclear_errno();
	*result = pow(value, exponent);
	return jerrno_is_clear();
}

bool
JExponent::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value, exponent;
	if (!GetArg1()->Evaluate(&value))
	{
		return false;
	}
	if (!GetArg2()->Evaluate(&exponent))
	{
		return false;
	}

	// avoid bug in g++ implementation
	if (real(value) == 0.0 && imag(value) == 0.0)
	{
		if (real(exponent) == 0.0 && imag(exponent) == 0.0)
		{
			*result = 1.0;		// 0 ^ 0
		}
		else
		{
			*result = 0.0;		// 0 ^ x, x != 0.0
		}
		return true;
	}
	else
	{
		jclear_errno();
		*result = pow(value, exponent);
		return jerrno_is_clear();
	}
}

/******************************************************************************
 Layout

 ******************************************************************************/

JIndex
JExponent::Layout
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	JPoint argUpperLeft = upperLeft;

	// get rectangle for base

	JFunction* base = GetArg1();
	const JIndex baseIndex =
		base->Layout(renderer, argUpperLeft, fontSize, rectList);

	JRect baseRect = rectList->GetRect(baseIndex);
	argUpperLeft.x = baseRect.right;

	if (JParenthesizeArgForRender(*this, *base))
	{
		const JSize parenWidth = renderer.GetParenthesisWidth(baseRect.height());
		rectList->ShiftRect(baseIndex, parenWidth, 0);
		baseRect = rectList->GetRect(baseIndex);
		argUpperLeft.x += 2*parenWidth;
	}

	// get rectangle for exponent

	const JSize expFontSize = renderer.GetSuperSubFontSize(fontSize);

	JFunction* exponent = GetArg2();
	const JIndex expIndex =
		exponent->Layout(renderer, argUpperLeft, expFontSize, rectList);
	const JRect expRect = rectList->GetRect(expIndex);

	// calculate our rectangle

	JRect ourRect;
	ourRect.top    = upperLeft.y;
	ourRect.left   = upperLeft.x;
	ourRect.bottom = ourRect.top + renderer.GetSuperscriptHeight(baseRect.height()) +
					 expRect.height();
	ourRect.right  = expRect.right;

	// shift the base down to the correct position inside ourRect

	if (ourRect.bottom > baseRect.bottom)
	{
		rectList->ShiftRect(baseIndex, 0, ourRect.bottom - baseRect.bottom);
	}
	else
	{
		ourRect.bottom = baseRect.bottom;
	}

	// save our rectangle

	const JCoordinate ourMidline = rectList->GetMidline(baseIndex);
	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JExponent::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	const JFunction* base = GetArg1();
	if (JParenthesizeArgForRender(*this, *base))
	{
		JIndex baseIndex;
		const bool found = rectList.FindFunction(base, &baseIndex);
		assert( found );
		renderer.DrawParentheses(rectList.GetRect(baseIndex));
	}

	base->Render(renderer, rectList);
	(GetArg2())->Render(renderer, rectList);
}
