/******************************************************************************
 JTangent.cpp

								The JTangent Class

	This class calculates the tangent of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JTangent.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTangent::JTangent()
	:
	JUnaryFunction(kJTangentNameIndex, kJTangentType)
{
}

JTangent::JTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJTangentNameIndex, kJTangentType)
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
	JTangent* newFunction = new JTangent(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JTangent::Evaluate
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
	*result = tan(argValue);
	return jerrno_is_clear();
}

JBoolean
JTangent::Evaluate
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
	*result = cos(argValue);
	if (real(*result) == 0.0 && imag(*result) == 0.0)
		{
		return kJFalse;
		}
	*result = sin(argValue)/(*result);
	return jerrno_is_clear();
}
