/******************************************************************************
 JHypTangent.cpp

	This class calculates the hyperbolic tangent of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JHypTangent.h"
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JHypTangent::JHypTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction("tanh", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JHypTangent::~JHypTangent()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JHypTangent::JHypTangent
	(
	const JHypTangent& source
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
JHypTangent::Copy()
	const
{
	auto* newFunction = jnew JHypTangent(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JHypTangent::Evaluate
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
	*result = tanh(argValue);
	return jerrno_is_clear();
}

bool
JHypTangent::Evaluate
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
	*result = sinh(argValue)/cosh(argValue);
	return jerrno_is_clear();
}
