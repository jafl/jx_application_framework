/******************************************************************************
 JTruncateToInt.cpp

	This class returns the integer part of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JTruncateToInt.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTruncateToInt::JTruncateToInt()
	:
	JUnaryFunction(kJTruncateNameIndex, kJTruncateType)
{
}

JTruncateToInt::JTruncateToInt
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJTruncateNameIndex, kJTruncateType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTruncateToInt::~JTruncateToInt()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTruncateToInt::JTruncateToInt
	(
	const JTruncateToInt& source
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
JTruncateToInt::Copy()
	const
{
	JTruncateToInt* newFunction = jnew JTruncateToInt(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JTruncateToInt::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value;
	if (!GetArg()->Evaluate(&value))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JTruncate(value);
	return jerrno_is_clear();
}

JBoolean
JTruncateToInt::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (!GetArg()->Evaluate(&value))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JComplex(JTruncate(real(value)), JTruncate(imag(value)));
	return jerrno_is_clear();
}
