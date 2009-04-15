/******************************************************************************
 JValueUnknown.cpp

							The Variable Value Unknown Class

	This class checks if the given variable's value is unknown.

	BASE CLASS = JDecisionWithVar

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JValueUnknown.h>
#include <JVariableList.h>
#include <JFunction.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JValueUnknown::JValueUnknown
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex
	)
	:
	JDecisionWithVar(theVariableList, variableIndex, arrayIndex, kJValueUnknownType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JValueUnknown::~JValueUnknown()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JValueUnknown::JValueUnknown
	(
	const JValueUnknown& source
	)
	:
	JDecisionWithVar(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JValueUnknown::Copy()
	const
{
	JValueUnknown* newDecision = new JValueUnknown(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JValueUnknown::Evaluate()
	const
{
	JIndex arrayIndex;
	if (EvaluateArrayIndex(&arrayIndex))
		{
		return (GetVariableList())->ValueIsUnknown(GetVariableIndex(), arrayIndex);
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
JValueUnknown::Print
	(
	ostream& output
	)
	const
{
	PrintVariable(output);
	output << JPGetValueUnknownString();
}
