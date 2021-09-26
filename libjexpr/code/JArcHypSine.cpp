/******************************************************************************
 JArcHypSine.cpp

	This class calculates the inverse hyperbolic sine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JArcHypSine.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

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
	auto* newFunction = jnew JArcHypSine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcHypSine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue))
	{
		return false;
	}
	jclear_errno();
	*result = asinh(argValue);
	return jerrno_is_clear();
}

bool
JArcHypSine::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!GetArg()->Evaluate(&argValue))
	{
		return false;
	}
	jclear_errno();
	*result = asinh(argValue);
	return jerrno_is_clear();
}
