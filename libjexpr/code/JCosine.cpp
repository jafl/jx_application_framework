/******************************************************************************
 JCosine.cpp

									The JCosine Class

	This class calculates the cosine of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JCosine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JCosine::JCosine()
	:
	JUnaryFunction(kJCosineNameIndex, kJCosineType)
{
}

JCosine::JCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJCosineNameIndex, kJCosineType)
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
	JCosine* newFunction = jnew JCosine(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JCosine::Evaluate
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
	*result = cos(argValue);
	return jerrno_is_clear();
}

JBoolean
JCosine::Evaluate
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
	*result = cos(argValue);
	return jerrno_is_clear();
}
