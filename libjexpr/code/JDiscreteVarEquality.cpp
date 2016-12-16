/******************************************************************************
 JDiscreteVarEquality.cpp

						The Discrete Variable Equality Class

	This class checks for equality between two discrete variables that have
	identical sets of values.

	BASE CLASS = JDecision

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JDiscreteVarEquality.h>
#include <JVariableList.h>
#include <JExprNodeList.h>
#include <JFunction.h>
#include <jParserData.h>
#include <JString.h>
#include <JOrderedSetUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Each arrayIndex can be NULL

 ******************************************************************************/

JDiscreteVarEquality::JDiscreteVarEquality
	(
	const JVariableList*	theVariableList,
	const JIndex			varIndex1,
	JFunction*				arrayIndex1,
	const JIndex			varIndex2,
	JFunction*				arrayIndex2
	)
	:
	JDecision(kJDiscreteVarEqualityType)
{
	itsVariableList  = theVariableList;
	itsVarIndex[0]   = varIndex1;
	itsArrayIndex[0] = arrayIndex1;
	itsVarIndex[1]   = varIndex2;
	itsArrayIndex[1] = arrayIndex2;

	assert( itsVariableList->IsDiscrete(itsVarIndex[0]) );
	assert( itsVariableList->IsDiscrete(itsVarIndex[1]) );
	assert( itsVariableList->HaveSameValues(itsVarIndex[0], itsVarIndex[1]) );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDiscreteVarEquality::~JDiscreteVarEquality()
{
	jdelete itsArrayIndex[0];
	jdelete itsArrayIndex[1];
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDiscreteVarEquality::JDiscreteVarEquality
	(
	const JDiscreteVarEquality& source
	)
	:
	JDecision(source)
{
	itsVariableList = source.itsVariableList;

	for (long i=0; i<2; i++)
		{
		itsVarIndex[i] = source.itsVarIndex[i];
		if (source.itsArrayIndex[i] != NULL)
			{
			itsArrayIndex[i] = (source.itsArrayIndex[i])->Copy();
			}
		else
			{
			itsArrayIndex[i] = NULL;
			}
		}
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JDiscreteVarEquality::Copy()
	const
{
	JDiscreteVarEquality* newDecision = jnew JDiscreteVarEquality(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JDiscreteVarEquality::Evaluate()
	const
{
	JIndex value[2];
	for (long i=0; i<2; i++)
		{
		JFloat x = 1.0;
		if (itsArrayIndex[i] != NULL && !itsArrayIndex[i]->Evaluate(&x))
			{
			return kJFalse;
			}
		JIndex arrayIndex = JRound(x);
		if (!itsVariableList->ArrayIndexValid(itsVarIndex[i], arrayIndex))
			{
			(JGetUserNotification())->ReportError("Array index out of bounds");
			return kJFalse;
			}
		value[i] = itsVariableList->GetDiscreteValue(itsVarIndex[i], arrayIndex);
		}
	return JConvertToBoolean( value[0] == value[1] );
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JDiscreteVarEquality::Print
	(
	std::ostream& output
	)
	const
{
	for (long i=0; i<2; i++)
		{
		const JString& varName = itsVariableList->GetVariableName(itsVarIndex[i]);
		varName.Print(output);

		if (itsArrayIndex[i] != NULL)
			{
			output << '[';
			itsArrayIndex[i]->Print(output);
			output << ']';
			}

		if (i == 0)
			{
			output << ' ' << JPGetEqualityString() << ' ';
			}
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.
	We don't compare the JVariableList pointers because this function can
	be called when comparing VariableLists.

 ******************************************************************************/

JBoolean
JDiscreteVarEquality::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (!JDecision::SameAs(theDecision))
		{
		return kJFalse;
		}

	const JDiscreteVarEquality& theDVE = (const JDiscreteVarEquality&) theDecision;

	// x=y is the same as both x=y and y=x

	JBoolean same[2] = {kJTrue, kJTrue};
	for (long j=0; j<2; j++)
		{
		for (long i=0; i<2; i++)
			{
			long k = i;
			if (j == 1)
				{
				k = 1-i;
				}

			if (itsVarIndex[i] != theDVE.itsVarIndex[k])
				{
				same[j] = kJFalse;
				break;
				}

			if (itsArrayIndex[i] == NULL && theDVE.itsArrayIndex[k] == NULL)
				{
				continue;
				}
			else if (itsArrayIndex[i] == NULL || theDVE.itsArrayIndex[k] == NULL)
				{
				same[j] = kJFalse;
				break;
				}
			else if (!itsArrayIndex[i]->SameAs(*(theDVE.itsArrayIndex[k])))
				{
				same[j] = kJFalse;
				break;
				}
			}
		}
	return JConvertToBoolean( same[0] || same[1] );
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JDiscreteVarEquality::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	if (itsArrayIndex[0] != NULL)
		{
		nodeList->RecurseNodesForFunction(myNode, itsArrayIndex[0]);
		}
	if (itsArrayIndex[1] != NULL)
		{
		nodeList->RecurseNodesForFunction(myNode, itsArrayIndex[1]);
		}
}

/******************************************************************************
 GetVariableIndex

 ******************************************************************************/

JIndex
JDiscreteVarEquality::GetVariableIndex
	(
	const JIndex index
	)
	const
{
	assert( VarIndexValid(index) );
	return itsVarIndex[ index-1 ];
}

/******************************************************************************
 SetVariableIndex

 ******************************************************************************/

void
JDiscreteVarEquality::SetVariableIndex
	(
	const JIndex index,
	const JIndex varIndex
	)
{
	assert( VarIndexValid(index) );
	assert( itsVariableList->IsDiscrete(varIndex) );

#ifndef NDEBUG
	{
	if (index == 1)
		{
		assert( itsVariableList->HaveSameValues(varIndex, itsVarIndex[1]) );
		}
	else if (index == 2)
		{
		assert( itsVariableList->HaveSameValues(itsVarIndex[0], varIndex) );
		}
	}
#endif

	itsVarIndex[ index-1 ] = varIndex;
}

/******************************************************************************
 GetArrayIndex

 ******************************************************************************/

const JFunction*
JDiscreteVarEquality::GetArrayIndex
	(
	const JIndex index
	)
	const
{
	assert( VarIndexValid(index) );
	return itsArrayIndex[ index-1 ];
}

/******************************************************************************
 SetArrayIndex

	Not inline to avoid including JFunction.h in header file

 ******************************************************************************/

void
JDiscreteVarEquality::SetArrayIndex
	(
	const JIndex	index,
	JFunction*		theArrayIndex
	)
{
	assert( VarIndexValid(index) );
	jdelete itsArrayIndex[ index-1 ];
	itsArrayIndex[ index-1 ] = theArrayIndex;
}

/******************************************************************************
 JVariableList messages (virtual)

 ******************************************************************************/

JBoolean
JDiscreteVarEquality::UsesVariable
	(
	const JIndex variableIndex
	)
	const
{
	return JI2B( itsVarIndex[0] == variableIndex ||
				 itsVarIndex[1] == variableIndex ||
				 JDecision::UsesVariable(variableIndex) );
}

void
JDiscreteVarEquality::VariablesInserted
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JDecision::VariablesRemoved(firstIndex, count);
	JAdjustIndexAfterInsert(firstIndex, count, &(itsVarIndex[0]));
	JAdjustIndexAfterInsert(firstIndex, count, &(itsVarIndex[1]));
}

void
JDiscreteVarEquality::VariablesRemoved
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	JDecision::VariablesRemoved(firstIndex, count);
	JBoolean ok = JAdjustIndexAfterRemove(firstIndex, count, &(itsVarIndex[0]));
	assert( ok );	// JVariableList must insure this
	ok = JAdjustIndexAfterRemove(firstIndex, count, &(itsVarIndex[1]));
	assert( ok );	// JVariableList must insure this
}

void
JDiscreteVarEquality::VariableMoved
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	JDecision::VariableMoved(origIndex, newIndex);
	JAdjustIndexAfterMove(origIndex, newIndex, &(itsVarIndex[0]));
	JAdjustIndexAfterMove(origIndex, newIndex, &(itsVarIndex[1]));
}

void
JDiscreteVarEquality::VariablesSwapped
	(
	const JIndex index1,
	const JIndex index2
	)
{
	JDecision::VariablesSwapped(index1, index2);
	JAdjustIndexAfterSwap(index1, index2, &(itsVarIndex[0]));
	JAdjustIndexAfterSwap(index1, index2, &(itsVarIndex[1]));
}
