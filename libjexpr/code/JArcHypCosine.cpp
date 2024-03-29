/******************************************************************************
 JArcHypCosine.cpp

	This class calculates the inverse hyperbolic cosine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JArcHypCosine.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypCosine::JArcHypCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arccosh", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcHypCosine::~JArcHypCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcHypCosine::JArcHypCosine
	(
	const JArcHypCosine& source
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
JArcHypCosine::Copy()
	const
{
	auto* newFunction = jnew JArcHypCosine(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcHypCosine::Evaluate
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
	*result = acosh(argValue);
	return jerrno_is_clear();
}

bool
JArcHypCosine::Evaluate
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
	*result = acosh(argValue);
	return jerrno_is_clear();
}
