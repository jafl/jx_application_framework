/******************************************************************************
 JPhaseAngle.cpp

	This class returns the phase angle of a JFunction in degrees.

	BASE CLASS = JUnaryFunction

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPhaseAngle.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JPhaseAngle::JPhaseAngle()
	:
	JUnaryFunction(kJPhaseAngleNameIndex, kJPhaseAngleType)
{
}

JPhaseAngle::JPhaseAngle
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJPhaseAngleNameIndex, kJPhaseAngleType)
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
	JPhaseAngle* newFunction = new JPhaseAngle(*this);
	assert( newFunction != NULL );
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
	if ((GetArg())->Evaluate(&value))
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
	if ((GetArg())->Evaluate(&value))
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
