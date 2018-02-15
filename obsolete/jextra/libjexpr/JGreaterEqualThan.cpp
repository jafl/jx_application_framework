/******************************************************************************
 JGreaterEqualThan.cpp

							The Greater Than Or Equal To Class

	This class checks if one JFunction is greater than or equal to another JFunction.

	BASE CLASS = JFunctionComparison

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JGreaterEqualThan.h>
#include <JLessEqualThan.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JGreaterEqualThan::JGreaterEqualThan()
	:
	JFunctionComparison(JPGetGreaterEqualThanString(), kJGreaterEqualThanType)
{
}

JGreaterEqualThan::JGreaterEqualThan
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JFunctionComparison(arg1, arg2, JPGetGreaterEqualThanString(), kJGreaterEqualThanType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGreaterEqualThan::~JGreaterEqualThan()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JGreaterEqualThan::JGreaterEqualThan
	(
	const JGreaterEqualThan& source
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
JGreaterEqualThan::Copy()
	const
{
	JGreaterEqualThan* newDecision = jnew JGreaterEqualThan(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JGreaterEqualThan::Evaluate()
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
	return JConvertToBoolean( result1 >= result2 );
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JGreaterEqualThan::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (JFunctionComparison::SameAsSameOrder(theDecision))
		{
		return kJTrue;
		}
	else if (theDecision.GetType() == kJLessEqualThanType)
		{
		const JLessEqualThan& theLET = (const JLessEqualThan&) theDecision;
		return JConvertToBoolean(
				(GetArg1())->SameAs(*(theLET.GetArg2())) &&
				(GetArg2())->SameAs(*(theLET.GetArg1())) );
		}
	else
		{
		return kJFalse;
		}
}
