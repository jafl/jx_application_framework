/******************************************************************************
 JArcHypTangent.cpp

	This class calculates the inverse hyperbolic tangent of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JArcHypTangent.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypTangent::JArcHypTangent()
	:
	JUnaryFunction(kJArcHypTangentNameIndex, kJArcHypTangentType)
{
}

JArcHypTangent::JArcHypTangent
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJArcHypTangentNameIndex, kJArcHypTangentType)
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
	JArcHypTangent* newFunction = new JArcHypTangent(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcHypTangent::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!(GetArg())->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JATanh(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcHypTangent::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!(GetArg())->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JATanh(argValue);
	return jerrno_is_clear();
}
