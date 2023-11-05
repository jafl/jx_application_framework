/******************************************************************************
 JArcCosine.cpp

								The JArcCosine Class

	This class calculates the arccos of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JArcCosine.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcCosine::JArcCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("arccos", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcCosine::~JArcCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcCosine::JArcCosine
	(
	const JArcCosine& source
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
JArcCosine::Copy()
	const
{
	auto* newFunction = jnew JArcCosine(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcCosine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue) || !(-1.0 <= argValue && argValue <= 1.0))
	{
		return false;
	}
	jclear_errno();
	*result = acos(argValue);
	return jerrno_is_clear();
}

bool
JArcCosine::Evaluate
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
	*result = acos(argValue);
	return jerrno_is_clear();
}
