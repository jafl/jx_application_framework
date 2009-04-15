/******************************************************************************
 JLessEqualThan.cpp

							The Less Than Or Equal To Class

	This class checks if one JFunction is less than or equal to another JFunction.

	BASE CLASS = JFunctionComparison

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JLessEqualThan.h>
#include <JGreaterEqualThan.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JLessEqualThan::JLessEqualThan()
	:
	JFunctionComparison(JPGetLessEqualThanString(), kJLessEqualThanType)
{
}

JLessEqualThan::JLessEqualThan
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JFunctionComparison(arg1, arg2, JPGetLessEqualThanString(), kJLessEqualThanType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JLessEqualThan::~JLessEqualThan()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JLessEqualThan::JLessEqualThan
	(
	const JLessEqualThan& source
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
JLessEqualThan::Copy()
	const
{
	JLessEqualThan* newDecision = new JLessEqualThan(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JLessEqualThan::Evaluate()
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
	return JConvertToBoolean( result1 <= result2 );
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JLessEqualThan::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (JFunctionComparison::SameAsSameOrder(theDecision))
		{
		return kJTrue;
		}
	else if (theDecision.GetType() == kJGreaterEqualThanType)
		{
		const JGreaterEqualThan& theGET = (const JGreaterEqualThan&) theDecision;
		return JConvertToBoolean(
				(GetArg1())->SameAs(*(theGET.GetArg2())) &&
				(GetArg2())->SameAs(*(theGET.GetArg1())) );
		}
	else
		{
		return kJFalse;
		}
}
