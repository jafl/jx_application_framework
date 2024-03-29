/******************************************************************************
 JAlgSign.cpp

	This class returns the algebraic sign (+1, 0, -1) of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JAlgSign.h"
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JAlgSign::JAlgSign
	(
	JFunction* arg
	)
	:
	JUnaryFunction("sign", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JAlgSign::~JAlgSign()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JAlgSign::JAlgSign
	(
	const JAlgSign& source
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
JAlgSign::Copy()
	const
{
	auto* newFunction = jnew JAlgSign(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JAlgSign::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value;
	if (!GetArg()->Evaluate(&value))
	{
		return false;
	}

	jclear_errno();
	*result = JSign(value);
	return jerrno_is_clear();
}

bool
JAlgSign::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (!GetArg()->Evaluate(&value))
	{
		return false;
	}

	jclear_errno();
	*result = JSign(real(value));
	return jerrno_is_clear();
}
