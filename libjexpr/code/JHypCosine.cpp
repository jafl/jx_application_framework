/******************************************************************************
 JHypCosine.cpp

	This class calculates the hyperbolic cosine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include <JHypCosine.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JHypCosine::JHypCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("cosh", arg)
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
	auto* newFunction = jnew JHypCosine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JHypCosine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue))
	{
		return false;
	}
	jclear_errno();
	*result = cosh(argValue);
	return jerrno_is_clear();
}

bool
JHypCosine::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!GetArg()->Evaluate(&argValue))
	{
		return false;
	}
	jclear_errno();
	*result = cosh(argValue);
	return jerrno_is_clear();
}
