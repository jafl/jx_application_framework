/******************************************************************************
 JVariableValue.cpp

								The Variable Value Class

	This class returns the value of a particular numeric variable.

	BASE CLASS = JFunctionWithVar

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JVariableValue.h>
#include <JVariableList.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JVariableValue::JVariableValue
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
	:
	JFunctionWithVar(theVariableList, variableIndex, arrayIndex,
					 kJVariableValueType)
{
	assert( theVariableList->IsNumeric(variableIndex) );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVariableValue::~JVariableValue()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JVariableValue::JVariableValue
	(
	const JVariableValue& source
	)
	:
	JFunctionWithVar(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JVariableValue::Copy()
	const
{
	JVariableValue* newFunction = new JVariableValue(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

	If the variable value is known, stores value in result and returns kJTrue.
	Otherwise, returns kJFalse.

 ******************************************************************************/

JBoolean
JVariableValue::Evaluate
	(
	JFloat* result
	)
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		return GetVariableList()->
			GetNumericValue(GetVariableIndex(), arrayIndex, result);
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JVariableValue::Evaluate
	(
	JComplex* result
	)
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		return GetVariableList()->
			GetNumericValue(GetVariableIndex(), arrayIndex, result);
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
JVariableValue::Print
	(
	ostream& output
	)
	const
{
	PrintVariable(output);
}

/******************************************************************************
 SetVariableIndex

 ******************************************************************************/

void
JVariableValue::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	assert( GetVariableList()->IsNumeric(variableIndex) );
	JFunctionWithVar::SetVariableIndex(variableIndex);
}
