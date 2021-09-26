/******************************************************************************
 JTangent.cpp

								The JTangent Class

	This class calculates the tangent of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JTangent.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTangent::JTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction("tan", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTangent::~JTangent()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTangent::JTangent
	(
	const JTangent& source
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
JTangent::Copy()
	const
{
	auto* newFunction = jnew JTangent(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JTangent::Evaluate
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
	*result = tan(argValue);
	return jerrno_is_clear();
}

bool
JTangent::Evaluate
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
	*result = cos(argValue);
	if (real(*result) == 0.0 && imag(*result) == 0.0)
	{
		return false;
	}
	*result = sin(argValue)/(*result);
	return jerrno_is_clear();
}
