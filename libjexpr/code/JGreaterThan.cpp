/******************************************************************************
 JGreaterThan.cpp

							The Greater Than Class

	This class checks if one JFunction is greater than another JFunction.

	BASE CLASS = JFunctionComparison

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JGreaterThan.h>
#include <JLessThan.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JGreaterThan::JGreaterThan()
	:
	JFunctionComparison(JPGetGreaterThanString(), kJGreaterThanType)
{
}

JGreaterThan::JGreaterThan
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JFunctionComparison(arg1, arg2, JPGetGreaterThanString(), kJGreaterThanType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGreaterThan::~JGreaterThan()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JGreaterThan::JGreaterThan
	(
	const JGreaterThan& source
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
JGreaterThan::Copy()
	const
{
	JGreaterThan* newDecision = new JGreaterThan(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JGreaterThan::Evaluate()
	const
{
	JFloat result1, result2;
	if (!(GetArg1())->Evaluate(&result1))
		{
		return kJFalse;
		}
	if (!(GetArg2())->Evaluate(&result2))
		{
		return kJFalse;
		}
	return JConvertToBoolean( result1 > result2 );
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JGreaterThan::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (JFunctionComparison::SameAsSameOrder(theDecision))
		{
		return kJTrue;
		}
	else if (theDecision.GetType() == kJLessThanType)
		{
		const JLessThan& theLT = (const JLessThan&) theDecision;
		return JConvertToBoolean(
				(GetArg1())->SameAs(*(theLT.GetArg2())) &&
				(GetArg2())->SameAs(*(theLT.GetArg1())) );
		}
	else
		{
		return kJFalse;
		}
}
