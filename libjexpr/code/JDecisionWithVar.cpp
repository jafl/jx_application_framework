/******************************************************************************
 JDecisionWithVar.cpp

						The Decision-with-a-Variable Class

	This abstract class provides routines for storing the reference to
	one variable.

	BASE CLASS = JDecision

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JDecisionWithVar.h>
#include <JVariableList.h>
#include <JExprNodeList.h>
#include <JFunction.h>
#include <JString.h>
#include <JOrderedSetUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	arrayIndex can be NULL

 ******************************************************************************/

JDecisionWithVar::JDecisionWithVar
	(
	const JVariableList*	theVariableList,
	const JIndex			variableIndex,
	JFunction*				arrayIndex,
	const JDecisionType		type
	)
	:
	JDecision(type)
{
	itsVariableList  = theVariableList;
	itsVariableIndex = variableIndex;
	itsArrayIndex    = arrayIndex;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDecisionWithVar::~JDecisionWithVar()
{
	delete itsArrayIndex;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDecisionWithVar::JDecisionWithVar
	(
	const JDecisionWithVar& source
	)
	:
	JDecision(source)
{
	itsVariableList  = source.itsVariableList;
	itsVariableIndex = source.itsVariableIndex;

	if (source.itsArrayIndex != NULL)
		{
		itsArrayIndex = (source.itsArrayIndex)->Copy();
		}
	else
		{
		itsArrayIndex = NULL;
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given decision is identical to us.
	We don't compare the JVariableList pointers because this function can
	be called when comparing VariableLists.

 ******************************************************************************/

JBoolean
JDecisionWithVar::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (!JDecision::SameAs(theDecision))
		{
		return kJFalse;
		}

	const JDecisionWithVar& theDecisionWithVar = (const JDecisionWithVar&) theDecision;

	if (itsVariableIndex != theDecisionWithVar.itsVariableIndex)
		{
		return kJFalse;
		}

	if (itsArrayIndex == NULL && theDecisionWithVar.itsArrayIndex == NULL)
		{
		return kJTrue;
		}
	else if (itsArrayIndex == NULL || theDecisionWithVar.itsArrayIndex == NULL)
		{
		return kJFalse;
		}
	else
		{
		return JConvertToBoolean( itsArrayIndex->SameAs(*(theDecisionWithVar.itsArrayIndex)) );
		}
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JDecisionWithVar::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	if (itsArrayIndex != NULL)
		{
		nodeList->RecurseNodesForFunction(myNode, itsArrayIndex);
		}
}

/******************************************************************************
 PrintVariable

 ******************************************************************************/

void
JDecisionWithVar::PrintVariable
	(
	ostream& output
	)
	const
{
	const JString& name = itsVariableList->GetVariableName(itsVariableIndex);
	name.Print(output);

	if (itsArrayIndex != NULL)
		{
		output << '[';
		itsArrayIndex->Print(output);
		output << ']';
		}
}

/******************************************************************************
 SetVariableIndex

	Derived classes can override this if they only allow certain types.
	Not inlined because it is virtual.

 ******************************************************************************/

void
JDecisionWithVar::SetVariableIndex
	(
	const JIndex variableIndex
	)
{
	itsVariableIndex = variableIndex;
}

/******************************************************************************
 SetArrayIndex

 ******************************************************************************/

void
JDecisionWithVar::SetArrayIndex
	(
	JFunction* theArrayIndex
	)
{
	delete itsArrayIndex;
	itsArrayIndex = theArrayIndex;
}

/******************************************************************************
 EvaluateArrayIndex

 ******************************************************************************/

JBoolean
JDecisionWithVar::EvaluateArrayIndex
	(
	JIndex* index
	)
	const
{
	JFloat x = 1.0;
	if (itsArrayIndex != NULL && !itsArrayIndex->Evaluate(&x))
		{
		return kJFalse;
		}
	*index = JRound(x);
	if (!itsVariableList->ArrayIndexValid(itsVariableIndex, *index))
		{
		(JGetUserNotification())->ReportError("Array index out of bounds");
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 JVariableList messages (virtual)

 ******************************************************************************/

JBoolean
JDecisionWithVar::UsesVariable
	(
	const JIndex variableIndex
	)
	const
{
	return JI2B( itsVariableIndex == variableIndex ||
				 JDecision::UsesVariable(variableIndex) );
}

void
JDecisionWithVar::VariablesInserted
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JDecision::VariablesRemoved(firstIndex, count);
	JAdjustIndexAfterInsert(firstIndex, count, &itsVariableIndex);
}

void
JDecisionWithVar::VariablesRemoved
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JDecision::VariablesRemoved(firstIndex, count);
	const JBoolean ok = JAdjustIndexAfterRemove(firstIndex, count, &itsVariableIndex);
	assert( ok );	// JVariableList must insure this
}

void
JDecisionWithVar::VariableMoved
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	JDecision::VariableMoved(origIndex, newIndex);
	JAdjustIndexAfterMove(origIndex, newIndex, &itsVariableIndex);
}

void
JDecisionWithVar::VariablesSwapped
	(
	const JIndex index1,
	const JIndex index2
	)
{
	JDecision::VariablesSwapped(index1, index2);
	JAdjustIndexAfterSwap(index1, index2, &itsVariableIndex);
}

/******************************************************************************
 Cast to JDecisionWithVar*

	Not inline because they are virtual

 ******************************************************************************/

JDecisionWithVar*
JDecisionWithVar::CastToJDecisionWithVar()
{
	return this;
}

const JDecisionWithVar*
JDecisionWithVar::CastToJDecisionWithVar()
	const
{
	return this;
}
