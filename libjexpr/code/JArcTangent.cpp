/******************************************************************************
 JArcTangent.cpp

								The JArcTangent Class

	This class calculates the arctan of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JArcTangent.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcTangent::JArcTangent()
	:
	JUnaryFunction(kJArcTangentNameIndex, kJArcTangentType)
{
}

JArcTangent::JArcTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJArcTangentNameIndex, kJArcTangentType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcTangent::~JArcTangent()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcTangent::JArcTangent
	(
	const JArcTangent& source
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
JArcTangent::Copy()
	const
{
	JArcTangent* newFunction = new JArcTangent(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcTangent::Evaluate
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
	*result = atan(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcTangent::Evaluate
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
	*result = JATan(argValue);
	return jerrno_is_clear();
}
