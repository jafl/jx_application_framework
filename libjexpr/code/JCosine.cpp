/******************************************************************************
 JCosine.cpp

									The JCosine Class

	This class calculates the cosine of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JCosine.h"
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JCosine::JCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("cos", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JCosine::~JCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JCosine::JCosine
	(
	const JCosine& source
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
JCosine::Copy()
	const
{
	auto* newFunction = jnew JCosine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JCosine::Evaluate
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
	*result = cos(argValue);
	return jerrno_is_clear();
}

bool
JCosine::Evaluate
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
	*result = cos(argValue);
	return jerrno_is_clear();
}
