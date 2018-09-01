/******************************************************************************
 jFunctionUtil.cpp

	Information used by all JFunctions.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jFunctionUtil.h"
#include "JConstantValue.h"
#include "JDivision.h"
#include "JExponent.h"
#include "JNegation.h"
#include "JParallel.h"
#include "JProduct.h"
#include "JSummation.h"
#include <jAssert.h>

/******************************************************************************
 Parenthesizing data

	operator i needs to parenthesize operator j
	index is [i*kFunctionParenTypeCount + j]

 ******************************************************************************/

enum JFunctionParenType
{
	kValueParenType = 0,		// values or normal functions f()
	kNegativeConstantType,
	kExponentParenType,
	kParallelParenType,
	kProductParenType,
	kDivisionParenType,
	kNegationParenType,
	kSummationParenType,

	kFunctionParenTypeCount
};

static const int kNeedParenForPrint[] =
{
	0, 0, 0, 0, 0, 0, 0, 0,	// value
	0, 0, 0, 0, 0, 0, 0, 0,	// negative constant
	0, 1, 1, 0, 1, 1, 1, 1,	// exponent
	0, 0, 0, 0, 0, 0, 0, 0,	// parallel
	0, 1, 0, 0, 0, 0, 1, 1,	// product
	0, 1, 1, 0, 1, 1, 1, 1,	// division
	0, 1, 0, 0, 0, 0, 1, 1,	// negation
	0, 1, 0, 0, 0, 0, 0, 0,	// summation
};

static const int kNeedParenForRender[] =
{
	0, 0, 0, 0, 0, 0, 0, 0,	// value
	0, 0, 0, 0, 0, 0, 0, 0,	// negative constant
	0, 1, 1, 1, 1, 1, 1, 1,	// exponent
	0, 1, 0, 1, 1, 0, 1, 1,	// parallel
	0, 1, 0, 1, 1, 0, 1, 1,	// product
	0, 0, 0, 0, 0, 0, 0, 0,	// division
	0, 1, 0, 0, 0, 0, 1, 1,	// negation
	0, 1, 0, 0, 0, 0, 0, 1,	// summation
};

/******************************************************************************
 GetParenType

 ******************************************************************************/

static JFunctionParenType
jGetParenType
	(
	const JFunction& f
	)
{
	if (dynamic_cast<const JExponent*>(&f) != nullptr)
		{
		return kExponentParenType;
		}
	else if (dynamic_cast<const JParallel*>(&f) != nullptr)
		{
		return kParallelParenType;
		}
	else if (dynamic_cast<const JProduct*>(&f) != nullptr)
		{
		return kProductParenType;
		}
	else if (dynamic_cast<const JDivision*>(&f) != nullptr)
		{
		return kDivisionParenType;
		}
	else if (dynamic_cast<const JNegation*>(&f) != nullptr)
		{
		return kNegationParenType;
		}
	else if (dynamic_cast<const JSummation*>(&f) != nullptr)
		{
		return kSummationParenType;
		}
	else if (dynamic_cast<const JConstantValue*>(&f) != nullptr &&
			 dynamic_cast<const JConstantValue*>(&f)->GetValue() < 0.0)
		{
		return kNegativeConstantType;
		}
	else
		{
		return kValueParenType;
		}
}

/******************************************************************************
 ParenthesizeArgForPrint

 ******************************************************************************/

JBoolean
JParenthesizeArgForPrint
	(
	const JFunction& f,
	const JFunction& arg
	)
{
	return JI2B(kNeedParenForPrint[ jGetParenType(f) * kFunctionParenTypeCount +
									jGetParenType(arg)] );
}

/******************************************************************************
 ParenthesizeArgForRender

 ******************************************************************************/

JBoolean
JParenthesizeArgForRender
	(
	const JFunction& f,
	const JFunction& arg
	)
{
	return JI2B(kNeedParenForRender[ jGetParenType(f) * kFunctionParenTypeCount +
									 jGetParenType(arg)] );
}
