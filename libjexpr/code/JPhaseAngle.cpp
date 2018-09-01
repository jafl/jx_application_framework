/******************************************************************************
 JPhaseAngle.cpp

	This class returns the phase angle of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include <JPhaseAngle.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JPhaseAngle::JPhaseAngle
	(
	JFunction* arg
	)
	:
	JUnaryFunction("phase", arg)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPhaseAngle::~JPhaseAngle()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JPhaseAngle::JPhaseAngle
	(
	const JPhaseAngle& source
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
JPhaseAngle::Copy()
	const
{
	JPhaseAngle* newFunction = jnew JPhaseAngle(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JPhaseAngle::Evaluate
	(
	JFloat* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		jclear_errno();
		*result = arg(value);
		return jerrno_is_clear();
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JPhaseAngle::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if (GetArg()->Evaluate(&value))
		{
		jclear_errno();
		*result = arg(value);
		return jerrno_is_clear();
		}
	else
		{
		return kJFalse;
		}
}
