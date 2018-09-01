/******************************************************************************
 JSine.cpp

									The JSine Class

	This class calculates the sine of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JSine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JSine::JSine
	(
	JFunction* arg
	)
	:
	JUnaryFunction("sin", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSine::~JSine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JSine::JSine
	(
	const JSine& source
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
JSine::Copy()
	const
{
	JSine* newFunction = jnew JSine(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JSine::Evaluate
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
	*result = sin(argValue);
	return jerrno_is_clear();
}

JBoolean
JSine::Evaluate
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
	*result = sin(argValue);
	return jerrno_is_clear();
}
