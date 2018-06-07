/******************************************************************************
 JAlgSign.cpp

	This class returns the algebraic sign (+1, 0, -1) of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JAlgSign.h>
#include <jMath.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JAlgSign::JAlgSign()
	:
	JUnaryFunction(kJSignNameIndex, kJSignType)
{
}

JAlgSign::JAlgSign
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJSignNameIndex, kJSignType)
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
	JAlgSign* newFunction = jnew JAlgSign(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JAlgSign::Evaluate
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
	*result = JSign(value);
	return jerrno_is_clear();
}

JBoolean
JAlgSign::Evaluate
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
	*result = JSign(real(value));
	return jerrno_is_clear();
}
