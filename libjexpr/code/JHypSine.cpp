/******************************************************************************
 JHypSine.cpp

	This class calculates the hyperbolic sine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JHypSine.h"
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JHypSine::JHypSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("sinh", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JHypSine::~JHypSine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JHypSine::JHypSine
	(
	const JHypSine& source
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
JHypSine::Copy()
	const
{
	auto* newFunction = jnew JHypSine(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JHypSine::Evaluate
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
	*result = sinh(argValue);
	return jerrno_is_clear();
}

bool
JHypSine::Evaluate
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
	*result = sinh(argValue);
	return jerrno_is_clear();
}
