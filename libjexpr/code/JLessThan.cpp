/******************************************************************************
 JLessThan.cpp

							The Less Than Class

	This class checks if one JFunction is less than another JFunction.

	BASE CLASS = JFunctionComparison

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JLessThan.h>
#include <JGreaterThan.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JLessThan::JLessThan()
	:
	JFunctionComparison(JPGetLessThanString(), kJLessThanType)
{
}

JLessThan::JLessThan
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JFunctionComparison(arg1, arg2, JPGetLessThanString(), kJLessThanType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JLessThan::~JLessThan()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JLessThan::JLessThan
	(
	const JLessThan& source
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
JLessThan::Copy()
	const
{
	JLessThan* newDecision = new JLessThan(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JLessThan::Evaluate()
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
	return JConvertToBoolean( result1 < result2 );
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JLessThan::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (JFunctionComparison::SameAsSameOrder(theDecision))
		{
		return kJTrue;
		}
	else if (theDecision.GetType() == kJGreaterThanType)
		{
		const JGreaterThan& theGT = (const JGreaterThan&) theDecision;
		return JConvertToBoolean(
				(GetArg1())->SameAs(*(theGT.GetArg2())) &&
				(GetArg2())->SameAs(*(theGT.GetArg1())) );
		}
	else
		{
		return kJFalse;
		}
}
