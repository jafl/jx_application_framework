/******************************************************************************
 JArcHypTangent.cpp

	This class calculates the inverse hyperbolic tangent of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JArcHypTangent.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypTangent::JArcHypTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arctanh", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcHypTangent::~JArcHypTangent()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcHypTangent::JArcHypTangent
	(
	const JArcHypTangent& source
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
JArcHypTangent::Copy()
	const
{
	auto* newFunction = jnew JArcHypTangent(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcHypTangent::Evaluate
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
	*result = atanh(argValue);
	return jerrno_is_clear();
}

bool
JArcHypTangent::Evaluate
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
	*result = atanh(argValue);
	return jerrno_is_clear();
}
