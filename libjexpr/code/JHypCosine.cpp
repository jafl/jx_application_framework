/******************************************************************************
 JHypCosine.cpp

	This class calculates the hyperbolic cosine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JHypCosine.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JHypCosine::JHypCosine()
	:
	JUnaryFunction(kJHypCosineNameIndex, kJHypCosineType)
{
}

JHypCosine::JHypCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJHypCosineNameIndex, kJHypCosineType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JHypCosine::~JHypCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JHypCosine::JHypCosine
	(
	const JHypCosine& source
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
JHypCosine::Copy()
	const
{
	JHypCosine* newFunction = jnew JHypCosine(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JHypCosine::Evaluate
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
	*result = cosh(argValue);
	return jerrno_is_clear();
}

JBoolean
JHypCosine::Evaluate
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
	*result = cosh(argValue);
	return jerrno_is_clear();
}
