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
#include <JStringMap.h>
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
 jGetParenType

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
 JParenthesizeArgForPrint

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
 JParenthesizeArgForRender

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

/******************************************************************************
 Function data

 ******************************************************************************/

struct FunctionData
{
	JSize argCount;
};

static JStringMap<FunctionData> theFunctionData;

inline void
jAddFunction
	(
	const JUtf8Byte*	name,
	const JSize			argCount
	)
{
	FunctionData data;
	data.argCount = argCount;

	const JBoolean added = theFunctionData.SetNewElement(JString(name, kJFalse), data);
	assert( added );
}

static void
jInitFunctionData()
{
	if (!theFunctionData.IsEmpty())
		{
		return;
		}

	jAddFunction("abs", 1);
	jAddFunction("sign", 1);
	jAddFunction("arccos", 1);
	jAddFunction("arccosh", 1);
	jAddFunction("arcsinh", 1);
	jAddFunction("arctanh", 1);
	jAddFunction("arcsin", 1);
	jAddFunction("arctan", 1);
	jAddFunction("arctan2", 2);
	jAddFunction("conjugate", 1);
	jAddFunction("cos", 1);
	jAddFunction("cosh", 1);
	jAddFunction("sinh", 1);
	jAddFunction("tanh", 1);
	jAddFunction("im", 1);
	jAddFunction("log", 2);
	jAddFunction("ln", 1);
	jAddFunction("max", 0);
	jAddFunction("min", 0);
	jAddFunction("parallel", 2);
	jAddFunction("arg", 1);
	jAddFunction("re", 1);
	jAddFunction("rotate", 2);
	jAddFunction("round", 1);
	jAddFunction("sin", 1);
	jAddFunction("sqrt", 1);
	jAddFunction("tan", 1);
	jAddFunction("truncate", 1);
}

/******************************************************************************
 JGetArgCount

 ******************************************************************************/

JBoolean
JGetArgCount
	(
	const JString&	fnName,
	JSize*			argCount
	)
{
	jInitFunctionData();

	FunctionData data;
	if (theFunctionData.GetElement(fnName, &data))
		{
		*argCount = data.argCount;
		return kJTrue;
		}
	else
		{
		*argCount = 0;
		return kJFalse;
		}
}
