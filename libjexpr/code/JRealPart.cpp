/******************************************************************************
 JRealPart.cpp

	This class returns the real part of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JRealPart.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JRealPart::JRealPart()
	:
	JUnaryFunction(kJRealPartNameIndex, kJRealPartType)
{
}

JRealPart::JRealPart
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJRealPartNameIndex, kJRealPartType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JRealPart::~JRealPart()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JRealPart::JRealPart
	(
	const JRealPart& source
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
JRealPart::Copy()
	const
{
	JRealPart* newFunction = new JRealPart(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JRealPart::Evaluate
	(
	JFloat* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		*result = real(value);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JRealPart::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		*result = real(value);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
