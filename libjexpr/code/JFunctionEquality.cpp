/******************************************************************************
 JFunctionEquality.cpp

							The JFunction Equality Class

	This class checks for equality between two Functions.

	BASE CLASS = JFunctionComparison

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFunctionEquality.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunctionEquality::JFunctionEquality()
	:
	JFunctionComparison(JPGetEqualityString(), kJFunctionEqualityType)
{
}

JFunctionEquality::JFunctionEquality
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JFunctionComparison(arg1, arg2, JPGetEqualityString(), kJFunctionEqualityType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunctionEquality::~JFunctionEquality()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JFunctionEquality::JFunctionEquality
	(
	const JFunctionEquality& source
	)
	:
	JFunctionComparison(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JFunctionEquality::Copy()
	const
{
	JFunctionEquality* newDecision = new JFunctionEquality(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JFunctionEquality::Evaluate()
	const
{
	JComplex result1, result2;
	if (!(GetArg1())->Evaluate(&result1))
		{
		return kJFalse;
		}
	if (!(GetArg2())->Evaluate(&result2))
		{
		return kJFalse;
		}
	return JConvertToBoolean( result1 == result2 );
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JFunctionEquality::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	return JFunctionComparison::SameAsEitherOrder(theDecision);
}
