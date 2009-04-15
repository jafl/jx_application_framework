/******************************************************************************
 JDiscreteEquality.cpp

							The Discrete Equality Class

	This class checks for equality between a discrete variable and a constant.

	BASE CLASS = JDecisionWithVar

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JDiscreteEquality.h>
#include <JVariableList.h>
#include <JFunction.h>
#include <jParserData.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JDiscreteEquality::JDiscreteEquality
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex,
	const JIndex			value
	)
	:
	JDecisionWithVar(theVariableList, variableIndex, arrayIndex, kJDiscreteEqualityType)
{
	itsValue = value;

	assert( theVariableList->IsDiscrete(variableIndex) );
	assert( ValueValid(value) );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDiscreteEquality::~JDiscreteEquality()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDiscreteEquality::JDiscreteEquality
	(
	const JDiscreteEquality& source
	)
	:
	JDecisionWithVar(source)
{
	itsValue = source.itsValue;
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JDiscreteEquality::Copy()
	const
{
	JDiscreteEquality* newDecision = new JDiscreteEquality(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JDiscreteEquality::Evaluate()
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		return JConvertToBoolean(
			(GetVariableList())->GetDiscreteValue(GetVariableIndex(), arrayIndex) == itsValue );
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JDiscreteEquality::Print
	(
	ostream& output
	)
	const
{
	PrintVariable(output);
	output << ' ' << JPGetEqualityString() << ' ';
	const JString& valueName =
		(GetVariableList())->GetDiscreteValueName(GetVariableIndex(), itsValue);
	valueName.Print(output);
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.
	We don't compare the JVariableList pointers because this function can
	be called when comparing VariableLists.

 ******************************************************************************/

JBoolean
JDiscreteEquality::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (!JDecisionWithVar::SameAs(theDecision))
		{
		return kJFalse;
		}

	const JDiscreteEquality& theDiscreteEquality = (const JDiscreteEquality&) theDecision;

	return JConvertToBoolean(itsValue == theDiscreteEquality.itsValue);
}

/******************************************************************************
 SetVariableIndex

 ******************************************************************************/

void
JDiscreteEquality::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	assert( (GetVariableList())->IsDiscrete(variableIndex) );
	JDecisionWithVar::SetVariableIndex(variableIndex);
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JDiscreteEquality::SetValue
	(
	const JIndex value
	)
{
	assert( ValueValid(value) );
	itsValue = value;
}

/******************************************************************************
 ValueValid (protected)

 ******************************************************************************/

JBoolean
JDiscreteEquality::ValueValid
	(
	const JIndex value
	)
	const
{
	return JConvertToBoolean( 1 <= value &&
				value <= (GetVariableList())->GetDiscreteValueCount(GetVariableIndex()) );
}
