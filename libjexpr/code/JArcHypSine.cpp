/******************************************************************************
 JArcHypSine.cpp

	This class calculates the inverse hyperbolic sine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JArcHypSine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypSine::JArcHypSine()
	:
	JUnaryFunction(kJArcHypSineNameIndex, kJArcHypSineType)
{
}

JArcHypSine::JArcHypSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJArcHypSineNameIndex, kJArcHypSineType)
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
	assert( newFunction != NULL );
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
