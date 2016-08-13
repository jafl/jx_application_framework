/******************************************************************************
 JDiscVarValueIndex.cpp

						The Discrete Variable Value Index Class

	This class returns the index of the value of a discrete variable.

	BASE CLASS = JFunctionWithVar

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JDiscVarValueIndex.h>
#include <JVariableList.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JDiscVarValueIndex::JDiscVarValueIndex
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
	:
	JFunctionWithVar(theVariableList, variableIndex, arrayIndex,
					 kJDiscVarValueIndexType)
{
	assert( theVariableList->IsDiscrete(variableIndex) );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDiscVarValueIndex::~JDiscVarValueIndex()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDiscVarValueIndex::JDiscVarValueIndex
	(
	const JDiscVarValueIndex& source
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
JDiscVarValueIndex::Copy()
	const
{
	JDiscVarValueIndex* newFunction = new JDiscVarValueIndex(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

	Returns the index of the variable's value.

 ******************************************************************************/

JBoolean
JDiscVarValueIndex::Evaluate
	(
	JFloat* result
	)
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		*result = GetVariableList()->GetDiscreteValue(GetVariableIndex(), arrayIndex);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JDiscVarValueIndex::Evaluate
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
JDiscVarValueIndex::Print
	(
	ostream& output
	)
	const
{
	const JFunctionPrintDest dest = GetPrintDestination();
	if (dest == kMathematica)
		{
		output << JPGetMathematicaDiscVarValueIndexString();
		}
	else
		{
		output << JPGetDiscVarValueIndexString();
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
JDiscVarValueIndex::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	assert( GetVariableList()->IsDiscrete(variableIndex) );
	JFunctionWithVar::SetVariableIndex(variableIndex);
}
