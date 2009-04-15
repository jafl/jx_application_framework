/******************************************************************************
 JDiscVarValue.cpp

						The Discrete Variable Value Class

	This class tries to returns the numeric value of a discrete variable.
	(i.e. the strings representing the values are actually numbers)

	BASE CLASS = JFunctionWithVar

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JDiscVarValue.h>
#include <JVariableList.h>
#include <jParserData.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JDiscVarValue::JDiscVarValue
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
	:
	JFunctionWithVar(theVariableList, variableIndex, arrayIndex, kJDiscVarValueType)
{
	assert( theVariableList->IsDiscrete(variableIndex) );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDiscVarValue::~JDiscVarValue()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDiscVarValue::JDiscVarValue
	(
	const JDiscVarValue& source
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
JDiscVarValue::Copy()
	const
{
	JDiscVarValue* newFunction = new JDiscVarValue(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

	If possible, returns the numeric value of the variable's string value.

 ******************************************************************************/

JBoolean
JDiscVarValue::Evaluate
	(
	JFloat* result
	)
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		const JVariableList* varList = GetVariableList();
		const JIndex varIndex        = GetVariableIndex();
		const JIndex valueIndex      = varList->GetDiscreteValue(varIndex, arrayIndex);
		const JString& valueName     = varList->GetDiscreteValueName(varIndex, valueIndex);
		return valueName.ConvertToFloat(result);
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JDiscVarValue::Evaluate
	(
	JComplex* result
	)
	const
{
	JFloat value;
	if (Evaluate(&value))
		{
		*result = value;
		return kJTrue;
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
JDiscVarValue::Print
	(
	ostream& output
	)
	const
{
	const JFunctionPrintDest dest = GetPrintDestination();
	if (dest == kMathematica)
		{
		output << JPGetMathematicaDiscVarValueString();
		}
	else
		{
		output << JPGetDiscVarValueString();
		}
	PrintVariable(output);
	if (dest == kMathematica)
		{
		output << ']';
		}
	else
		{
		output << ')';
		}
}

/******************************************************************************
 SetVariableIndex

 ******************************************************************************/

void
JDiscVarValue::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	assert( (GetVariableList())->IsDiscrete(variableIndex) );
	JFunctionWithVar::SetVariableIndex(variableIndex);
}
