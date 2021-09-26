/******************************************************************************
 JArcSine.cpp

								The JArcSine Class

	This class calculates the arcsin of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JArcSine.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

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
	auto* newFunction = jnew JArcSine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcSine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue) || !(-1.0 <= argValue && argValue <= 1.0))
	{
		return false;
	}
	jclear_errno();
	*result = asin(argValue);
	return jerrno_is_clear();
}

bool
JArcSine::Evaluate
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
	*result = asin(argValue);
	return jerrno_is_clear();
}
