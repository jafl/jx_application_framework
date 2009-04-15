/******************************************************************************
 JRotateComplex.cpp

	This class rotates the given complex number by the specified number
	of degrees.

	BASE CLASS = JBinaryFunction

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JRotateComplex.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JRotateComplex::JRotateComplex()
	:
	JBinaryFunction(kJRotateNameIndex, kJRotateType)
{
}

JRotateComplex::JRotateComplex
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryFunction(arg1, arg2, kJRotateNameIndex, kJRotateType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JRotateComplex::~JRotateComplex()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JRotateComplex::JRotateComplex
	(
	const JRotateComplex& source
	)
	:
	JBinaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JRotateComplex::Copy()
	const
{
	JRotateComplex* newFunction = new JRotateComplex(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JRotateComplex::Evaluate
	(
	JFloat* result
	)
	const
{
	*result = 0.0;
	return kJFalse;
}

JBoolean
JRotateComplex::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex x;
	if (!(GetArg1())->Evaluate(&x))
		{
		return kJFalse;
		}

	JFloat a;
	if (!(GetArg2())->Evaluate(&a))
		{
		return kJFalse;
		}

	jclear_errno();
	*result = x * exp(JComplex(0.0, a));
	return jerrno_is_clear();
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JRotateComplex::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JBinaryFunction::SameAsSameOrder(theFunction);
}
