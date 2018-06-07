/******************************************************************************
 JMinFunc.cpp

								The Minimum JFunction Class

	This class returns the minimum of its arguments.

	BASE CLASS = JNaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JMinFunc.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JMinFunc::JMinFunc()
	:
	JNaryFunction(kJMinFuncNameIndex, kJMinFuncType)
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
	JMinFunc* newFunction = jnew JMinFunc(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JMinFunc::Evaluate
	(
	JFloat* result
	)
	const
{
	// start with the value of the first argument

	if (GetArgCount() == 0 || !(GetArg(1))->Evaluate(result))
		{
		return kJFalse;
		}

	// find the minimum of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
		{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		if (argValue < *result)
			{
			*result = argValue;
			}
		}
	return kJTrue;
}

JBoolean
JMinFunc::Evaluate
	(
	JComplex* result
	)
	const
{
	// start with the value of the first argument

	if (GetArgCount() == 0 || !(GetArg(1))->Evaluate(result))
		{
		return kJFalse;
		}

	// find the minimum real part of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
		{
		JComplex argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		if (real(argValue) < real(*result))
			{
			*result = argValue;
			}
		}

	*result = real(*result);
	return kJTrue;
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JMinFunc::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JNaryFunction::SameAsAnyOrder(theFunction);
}
