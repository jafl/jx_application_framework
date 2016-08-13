/******************************************************************************
 JImagPart.cpp

	This class returns the imaginary part of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JImagPart.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JImagPart::JImagPart()
	:
	JUnaryFunction(kJImagPartNameIndex, kJImagPartType)
{
}

JImagPart::JImagPart
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJImagPartNameIndex, kJImagPartType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JImagPart::~JImagPart()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JImagPart::JImagPart
	(
	const JImagPart& source
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
JImagPart::Copy()
	const
{
	JImagPart* newFunction = new JImagPart(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JImagPart::Evaluate
	(
	JFloat* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		*result = imag(value);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JImagPart::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		*result = imag(value);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
