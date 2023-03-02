/******************************************************************************
 JLogE.cpp

								The log base e Class

	This class calculates the log of a JFunction base e.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JLogE.h"
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JLogE::JLogE
	(
	JFunction* arg
	)
	:
	JUnaryFunction("ln", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JLogE::~JLogE()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JLogE::JLogE
	(
	const JLogE& source
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
JLogE::Copy()
	const
{
	auto* newFunction = jnew JLogE(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JLogE::Evaluate
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
	*result = log(argValue);
	return jerrno_is_clear();
}

bool
JLogE::Evaluate
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
	*result = log(argValue);
	return jerrno_is_clear();
}
