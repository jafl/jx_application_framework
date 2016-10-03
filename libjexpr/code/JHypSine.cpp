/******************************************************************************
 JHypSine.cpp

	This class calculates the hyperbolic sine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JHypSine.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JHypSine::JHypSine()
	:
	JUnaryFunction(kJHypSineNameIndex, kJHypSineType)
{
}

JHypSine::JHypSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJHypSineNameIndex, kJHypSineType)
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
	JHypSine* newFunction = new JHypSine(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JHypSine::Evaluate
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
	*result = sinh(argValue);
	return jerrno_is_clear();
}

JBoolean
JHypSine::Evaluate
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
	*result = sinh(argValue);
	return jerrno_is_clear();
}
