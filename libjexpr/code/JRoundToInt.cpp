/******************************************************************************
 JRoundToInt.cpp

	This class returns the integer nearest the value of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JRoundToInt.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JRoundToInt::JRoundToInt
	(
	JFunction* arg
	)
	:
	JUnaryFunction("round", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JRoundToInt::~JRoundToInt()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JRoundToInt::JRoundToInt
	(
	const JRoundToInt& source
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
JRoundToInt::Copy()
	const
{
	auto* newFunction = jnew JRoundToInt(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JRoundToInt::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value;
	if (GetArg()->Evaluate(&value))
	{
		jclear_errno();
		*result = JRound(value);
		return jerrno_is_clear();
	}
	else
	{
		return false;
	}
}

bool
JRoundToInt::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
	{
		jclear_errno();
		*result = JComplex(JRound(real(value)), JRound(imag(value)));
		return jerrno_is_clear();
	}
	else
	{
		return false;
	}
}
