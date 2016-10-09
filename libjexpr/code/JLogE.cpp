/******************************************************************************
 JLogE.cpp

								The log base e Class

	This class calculates the log of a JFunction base e.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JLogE.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JLogE::JLogE()
	:
	JUnaryFunction(kJLogENameIndex, kJLogEType)
{
}

JLogE::JLogE
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJLogENameIndex, kJLogEType)
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
	JLogE* newFunction = jnew JLogE(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JLogE::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!GetArg()->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = log(argValue);
	return jerrno_is_clear();
}

JBoolean
JLogE::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!GetArg()->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = log(argValue);
	return jerrno_is_clear();
}
