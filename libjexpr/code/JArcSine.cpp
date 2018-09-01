/******************************************************************************
 JArcSine.cpp

								The JArcSine Class

	This class calculates the arcsin of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JArcSine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcSine::JArcSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arcsin", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcSine::~JArcSine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcSine::JArcSine
	(
	const JArcSine& source
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
JArcSine::Copy()
	const
{
	JArcSine* newFunction = jnew JArcSine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcSine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = asin(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcSine::Evaluate
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
	*result = JASin(argValue);
	return jerrno_is_clear();
}
