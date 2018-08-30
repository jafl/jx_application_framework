/******************************************************************************
 JMaxFunc.cpp

								The Maximum JFunction Class

	This class returns the maximum of its arguments.

	BASE CLASS = JNaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JMaxFunc.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JMaxFunc::JMaxFunc
	(
	JPtrArray<JFunction>* argList
	)
	:
	JNaryFunction(kJMaxFuncNameIndex, kJMaxFuncType, argList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JMaxFunc::~JMaxFunc()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JMaxFunc::JMaxFunc
	(
	const JMaxFunc& source
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
JMaxFunc::Copy()
	const
{
	JMaxFunc* newFunction = jnew JMaxFunc(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JMaxFunc::Evaluate
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

	// find the maximum of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
		{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		if (argValue > *result)
			{
			*result = argValue;
			}
		}
	return kJTrue;
}

JBoolean
JMaxFunc::Evaluate
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

	// find the maximum real part of all the arguments

	const JSize argCount = GetArgCount();
	for (JIndex i=2; i<=argCount; i++)
		{
		JComplex argValue;
		if (!(GetArg(i))->Evaluate(&argValue))
			{
			return kJFalse;
			}
		if (real(argValue) > real(*result))
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
JMaxFunc::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JNaryFunction::SameAsAnyOrder(theFunction);
}
