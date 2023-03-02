/******************************************************************************
 JRealPart.cpp

	This class returns the real part of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JRealPart.h"
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JRealPart::JRealPart
	(
	JFunction* arg
	)
	:
	JUnaryFunction("re", arg)
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
	auto* newFunction = jnew JRealPart(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
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
		return true;
	}
	else
	{
		return false;
	}
}

bool
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
		return true;
	}
	else
	{
		return false;
	}
}
