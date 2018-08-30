/******************************************************************************
 JArcHypSine.cpp

	This class calculates the inverse hyperbolic sine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JArcHypSine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypSine::JArcHypSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arcsinh", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcHypSine::~JArcHypSine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcHypSine::JArcHypSine
	(
	const JArcHypSine& source
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
JArcHypSine::Copy()
	const
{
	JArcHypSine* newFunction = jnew JArcHypSine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcHypSine::Evaluate
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
	*result = JASinh(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcHypSine::Evaluate
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
	*result = JASinh(argValue);
	return jerrno_is_clear();
}
