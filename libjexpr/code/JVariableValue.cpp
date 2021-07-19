/******************************************************************************
 JVariableValue.cpp

								The Variable Value Class

	This class returns the value of a particular numeric variable.

	BASE CLASS = JFunctionWithVar

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JVariableValue.h>
#include <JVariableList.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be nullptr

 ******************************************************************************/

JVariableValue::JVariableValue
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
	:
	JFunctionWithVar(theVariableList, variableIndex, arrayIndex)
{
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
	auto* newFunction = jnew JVariableValue(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

	If the variable value is known, stores value in result and returns true.
	Otherwise, returns false.

 ******************************************************************************/

bool
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
		return false;
		}
}

bool
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
		return false;
		}
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JVariableValue::Print
	(
	std::ostream& output
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
	JFunctionWithVar::SetVariableIndex(variableIndex);
}
