/******************************************************************************
 JImagPart.cpp

	This class returns the imaginary part of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include <JImagPart.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JImagPart::JImagPart
	(
	JFunction* arg
	)
	:
	JUnaryFunction("im", arg)
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
	auto* newFunction = jnew JImagPart(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
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
		return true;
		}
	else
		{
		return false;
		}
}

bool
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
		return true;
		}
	else
		{
		return false;
		}
}
