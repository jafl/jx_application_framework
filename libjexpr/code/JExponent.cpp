/******************************************************************************
 JExponent.cpp

								The JExponent Class

	This class computes (arg1)^arg2.

	BASE CLASS = JBinaryOperator

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JExponent.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JRect.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JExponent::JExponent()
	:
	JBinaryOperator(kJExponentNameIndex, kJExponentType)
{
}

JExponent::JExponent
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryOperator(arg1, arg2, kJExponentNameIndex, kJExponentType)
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
	JExponent* newFunction = new JExponent(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JExponent::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value, exponent;
	if (!(GetArg1())->Evaluate(&value))
		{
		return kJFalse;
		}
	if (!(GetArg2())->Evaluate(&exponent))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = pow(value, exponent);
	return jerrno_is_clear();
}

JBoolean
JExponent::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value, exponent;
	if (!(GetArg1())->Evaluate(&value))
		{
		return kJFalse;
		}
	if (!(GetArg2())->Evaluate(&exponent))
		{
		return kJFalse;
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
		return kJTrue;
		}
	else
		{
		jclear_errno();
		*result = pow(value, exponent);
		return jerrno_is_clear();
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JExponent::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JBinaryOperator::SameAsSameOrder(theFunction);
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JExponent::PrepareToRender
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
		base->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);

	JRect baseRect = rectList->GetRect(baseIndex);
	argUpperLeft.x = baseRect.right;

	if (ParenthesizeArgForRender(*this, *base))
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
		exponent->PrepareToRender(renderer, argUpperLeft, expFontSize, rectList);
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
	if (ParenthesizeArgForRender(*this, *base))
		{
		JIndex baseIndex;
		const JBoolean found = rectList.FindFunction(base, &baseIndex);
		assert( found );
		renderer.DrawParentheses(rectList.GetRect(baseIndex));
		}

	base->Render(renderer, rectList);
	(GetArg2())->Render(renderer, rectList);
}
