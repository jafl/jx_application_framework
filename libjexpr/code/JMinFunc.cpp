/******************************************************************************
 JMinFunc.cpp

								The Minimum JFunction Class

	This class returns the minimum of its arguments.

	BASE CLASS = JNaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JMinFunc.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JMinFunc::JMinFunc
	(
	JPtrArray<JFunction>* argList
	)
	:
	JNaryFunction("min", argList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JMinFunc::~JMinFunc()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JMinFunc::JMinFunc
	(
	const JMinFunc& source
	)
	:
	JNaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JMinFunc::Copy()
	const
{
	auto* newFunction = jnew JMinFunc(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JMinFunc::Evaluate
	(
	JFloat* result
	)
	const
{
	// start with the value of the first argument

	if (GetArgCount() == 0 || !(GetArg(1))->Evaluate(result))
	{
		return false;
	}

	// find the minimum of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
	{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
		{
			return false;
		}
		if (argValue < *result)
		{
			*result = argValue;
		}
	}
	return true;
}

bool
JMinFunc::Evaluate
	(
	JComplex* result
	)
	const
{
	// start with the value of the first argument

	if (GetArgCount() == 0 || !(GetArg(1))->Evaluate(result))
	{
		return false;
	}

	// find the minimum real part of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
	{
		JComplex argValue;
		if (!GetArg(i)->Evaluate(&argValue))
		{
			return false;
		}
		if (real(argValue) < real(*result))
		{
			*result = argValue;
		}
	}

	*result = real(*result);
	return true;
}
