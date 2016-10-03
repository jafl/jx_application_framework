/******************************************************************************
 JFunctionData.cpp

	Information used by all JFunctions.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFunction.h>
#include <JConstantValue.h>
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

JFunctionParenType GetParenType(const JFunction& f);

JFunctionParenType
GetParenType
	(
	const JFunction& f
	)
{
	const JFunctionType fnType = f.GetType();

	if (fnType == kJExponentType)
		{
		return kExponentParenType;
		}
	else if (fnType == kJParallelType)
		{
		return kParallelParenType;
		}
	else if (fnType == kJProductType)
		{
		return kProductParenType;
		}
	else if (fnType == kJDivisionType)
		{
		return kDivisionParenType;
		}
	else if (fnType == kJNegationType)
		{
		return kNegationParenType;
		}
	else if (fnType == kJSummationType)
		{
		return kSummationParenType;
		}
	else if (fnType == kJConstantValueType &&
			 ((const JConstantValue&) f).GetValue() < 0.0)
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
ParenthesizeArgForPrint
	(
	const JFunction& f,
	const JFunction& arg
	)
{
	return JConvertToBoolean(
				kNeedParenForPrint[ GetParenType(f) * kFunctionParenTypeCount +
									GetParenType(arg)] );
}

/******************************************************************************
 ParenthesizeArgForRender

 ******************************************************************************/

JBoolean
ParenthesizeArgForRender
	(
	const JFunction& f,
	const JFunction& arg
	)
{
	return JConvertToBoolean(
				kNeedParenForRender[ GetParenType(f) * kFunctionParenTypeCount +
									 GetParenType(arg)] );
}
