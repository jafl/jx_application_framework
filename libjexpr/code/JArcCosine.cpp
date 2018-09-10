/******************************************************************************
 JArcCosine.cpp

								The JArcCosine Class

	This class calculates the arccos of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JArcCosine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcCosine::JArcCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arccos", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcCosine::~JArcCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcCosine::JArcCosine
	(
	const JArcCosine& source
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
JArcCosine::Copy()
	const
{
	JArcCosine* newFunction = jnew JArcCosine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcCosine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue) || !(-1.0 <= argValue && argValue <= 1.0))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = acos(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcCosine::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!GetArg()->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JACos(argValue);
	return jerrno_is_clear();
}
