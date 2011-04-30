/******************************************************************************
 JVariableList.cpp

							The JVariableList Class

	This abstract class defines the interface for storing a set of variables
	and their values for use by the parsing system.

	*** Derived classes must support the JBroadcaster messages.

	We require that forms of GetNumericValue() be implemented by the derived
	class because we do not know the best way to store the values.  Some may
	choose to store everything as JComplex.  In this case,
	GetNumericValue(..., JFloat*) can either return the real part or return
	kJFalse.  Others may choose to store a mixture of JFloat and JComplex.
	In some applications, one may even store only JFloat because one never uses
	JComplex at all.

	SetNumericValue() and SetDiscreteValue() should assert if they are called
	incorrectly because it is the client's responsibility to call them only
	when appropriate.  IsNumeric(), IsArray(), and ArrayIndexValid() are
	available so the client can figure this out.

	By providing functions for keeping an evalutation stack, we allow derived
	classes to catch variables defined in terms of themselves and to avoid the
	resulting infinite recursion that GetNumericValue() would get stuck in.
	The stack is implemented as an array of JBoolean so all operations are
	merely array look-ups.

	Notes:

	An array is either all numeric or all discrete.

	If a discrete variable is an array, it is assumed that each element of
	the array uses the same set of value names.

	BASE CLASS = JContainer

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JVariableList.h>
#include <JDecision.h>
#include <JFunction.h>
#include <jParseUtil.h>
#include <JString.h>
#include <JOrderedSet.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JVariableList::kVarInserted          = "VarInserted::JVariableList";
const JCharacter* JVariableList::kVarRemoved           = "VarRemoved::JVariableList";
const JCharacter* JVariableList::kVarMoved             = "VarMoved::JVariableList";
const JCharacter* JVariableList::kVarNameChanged       = "VarNameChanged::JVariableList";
const JCharacter* JVariableList::kVarValueChanged      = "VarValueChanged::JVariableList";
const JCharacter* JVariableList::kDiscValueNameChanged = "DiscValueNameChanged::JVariableList";

/******************************************************************************
 Constructor

 ******************************************************************************/

JVariableList::JVariableList()
	:
	JContainer()
{
	JVariableListX();
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JVariableList::JVariableList
	(
	const JVariableList& source
	)
	:
	JContainer(source)
{
	JVariableListX();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVariableList::~JVariableList()
{
	delete itsDVarUserList;		// objects deleted by owners
	delete itsFVarUserList;		// objects deleted by owners
	delete itsEvalStack;
}

/******************************************************************************
 JVariableListX

 ******************************************************************************/

void
JVariableList::JVariableListX()
{
	itsDVarUserList = new JPtrArray<JDecision>(JPtrArrayT::kForgetAll);
	assert( itsDVarUserList != NULL );

	itsFVarUserList = new JPtrArray<JFunction>(JPtrArrayT::kForgetAll);
	assert( itsFVarUserList != NULL );

	itsEvalStack = new JArray<JBoolean>(10);
	assert( itsEvalStack != NULL );
}

/******************************************************************************
 ParseVariableName

	Returns kJTrue if expr is the name of a variable.

 ******************************************************************************/

JBoolean
JVariableList::ParseVariableName
	(
	const JCharacter*	expr,
	const JSize			exprLength,
	JIndex*				index
	)
	const
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString& name = GetVariableName(i);
		if (JStringsEqual(expr, exprLength, name))
			{
			*index = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 FindUniqueVarName

	Checks if prefix is the beginning of a variable name.

	If not, returns kNoMatch.
	If there is a single match, returns the index and the variable name.
	If there are multiple matches, returns index=0 and the most characters
		that are common to all the matches.

	If there is an exact match, it is returned as if there was a single match.

 ******************************************************************************/

JVariableList::MatchResult
JVariableList::FindUniqueVarName
	(
	const JCharacter*	prefix,
	JIndex*				index,
	JString*			maxPrefix
	)
	const
{
	assert( !JStringEmpty(prefix) );

	const JSize count = GetElementCount();
	JArray<JIndex> matchList;

	for (JIndex i=1; i<=count; i++)
		{
		const JString& name = GetVariableName(i);
		if (name == prefix)
			{
			*index     = i;
			*maxPrefix = name;
			return kSingleMatch;
			}
		else if (JStringBeginsWith(name, name.GetLength(), prefix))
			{
			matchList.AppendElement(i);
			}
		}

	const JSize matchCount = matchList.GetElementCount();
	if (matchCount == 0)
		{
		*index = 0;
		maxPrefix->Clear();
		return kNoMatch;
		}
	else if (matchCount == 1)
		{
		*index     = matchList.GetElement(1);
		*maxPrefix = GetVariableName(*index);
		return kSingleMatch;
		}
	else
		{
		*maxPrefix = GetVariableName( matchList.GetElement(1) );
		for (JIndex i=2; i<=matchCount; i++)
			{
			const JString& varName   = GetVariableName( matchList.GetElement(i) );
			const JSize matchLength  = JCalcMatchLength(*maxPrefix, varName);
			const JSize prefixLength = maxPrefix->GetLength();
			if (matchLength < prefixLength)
				{
				maxPrefix->RemoveSubstring(matchLength+1, prefixLength);
				}
			}
		*index = 0;
		return kMultipleMatch;
		}
}

/******************************************************************************
 ParseDiscreteValue

	Returns kJTrue if expr is the name of a value for the given variable.

 ******************************************************************************/

JBoolean
JVariableList::ParseDiscreteValue
	(
	const JCharacter*	expr,
	const JSize			exprLength,
	const JIndex&		variableIndex,
	JIndex*				valueIndex
	)
	const
{
	const JSize count = GetDiscreteValueCount(variableIndex);

	for (JIndex i=1; i<=count; i++)
		{
		const JString& name = GetDiscreteValueName(variableIndex, i);
		if (JStringsEqual(expr, exprLength, name))
			{
			*valueIndex = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HaveSameValues

 ******************************************************************************/

JBoolean
JVariableList::HaveSameValues
	(
	const JIndex index1,
	const JIndex index2
	)
	const
{
	if (IsNumeric(index1) && IsNumeric(index2))
		{
		return kJTrue;
		}
	else if (IsDiscrete(index1) && IsDiscrete(index2))
		{
		const JSize valueCount = GetDiscreteValueCount(index1);
		if (valueCount != GetDiscreteValueCount(index2))
			{
			return kJFalse;
			}
		for (JIndex i=1; i<=valueCount; i++)
			{
			const JString& value1 = GetDiscreteValueName(index1, i);
			const JString& value2 = GetDiscreteValueName(index2, i);
			if (value1 != value2)
				{
				return kJFalse;
				}
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PushOnEvalStack (protected)

 ******************************************************************************/

void
JVariableList::PushOnEvalStack
	(
	const JIndex variableIndex
	)
	const
{
	assert( !IsOnEvalStack(variableIndex) );

	itsEvalStack->SetElement(variableIndex, kJTrue);
}

/******************************************************************************
 PopOffEvalStack (protected)

 ******************************************************************************/

void
JVariableList::PopOffEvalStack
	(
	const JIndex variableIndex
	)
	const
{
	assert( IsOnEvalStack(variableIndex) );

	itsEvalStack->SetElement(variableIndex, kJFalse);
}

/******************************************************************************
 OKToRemoveVariable

	Never delete a variable from the list without checking this!

 ******************************************************************************/

JBoolean
JVariableList::OKToRemoveVariable
	(
	const JIndex variableIndex
	)
	const
{
JIndex i;

	const JSize dCount = itsDVarUserList->GetElementCount();
	for (i=1; i<=dCount; i++)
		{
		if ((itsDVarUserList->NthElement(i))->UsesVariable(variableIndex))
			{
			return kJFalse;
			}
		}

	const JSize fCount = itsFVarUserList->GetElementCount();
	for (i=1; i<=fCount; i++)
		{
		if ((itsFVarUserList->NthElement(i))->UsesVariable(variableIndex))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 Reference counting

	These functions are const because the information isn't really
	part of the object.  It's just machinery behind the scenes.

 ******************************************************************************/

void
JVariableList::VariableUserCreated
	(
	JDecision* d
	)
	const
{
	itsDVarUserList->Append(d);
}

void
JVariableList::VariableUserDeleted
	(
	JDecision* d
	)
	const
{
	itsDVarUserList->Remove(d);
}

void
JVariableList::VariableUserCreated
	(
	JFunction* f
	)
	const
{
	itsFVarUserList->Append(f);
}

void
JVariableList::VariableUserDeleted
	(
	JFunction* f
	)
	const
{
	itsFVarUserList->Remove(f);
}

/******************************************************************************
 VariablesInserted (private)

 ******************************************************************************/

void
JVariableList::VariablesInserted
	(
	const JOrderedSetT::ElementsInserted& info
	)
	const
{
JIndex i;

	for (i=1; i<=info.GetCount(); i++)
		{
		itsEvalStack->InsertElementAtIndex(info.GetFirstIndex(), kJFalse);
		}

	const JSize dCount = itsDVarUserList->GetElementCount();
	for (i=1; i<=dCount; i++)
		{
		(itsDVarUserList->NthElement(i))->
			VariablesInserted(info.GetFirstIndex(), info.GetCount());
		}

	const JSize fCount = itsFVarUserList->GetElementCount();
	for (i=1; i<=fCount; i++)
		{
		(itsFVarUserList->NthElement(i))->
			VariablesInserted(info.GetFirstIndex(), info.GetCount());
		}
}

/******************************************************************************
 VariablesRemoved (private)

 ******************************************************************************/

void
JVariableList::VariablesRemoved
	(
	const JOrderedSetT::ElementsRemoved& info
	)
	const
{
JIndex i;

	#ifndef NDEBUG
	{
	for (i=info.GetFirstIndex(); i<=info.GetLastIndex(); i++)
		{
		assert( OKToRemoveVariable(i) );
		}
	}
	#endif

	itsEvalStack->RemoveNextElements(info.GetFirstIndex(), info.GetCount());

	const JSize dCount = itsDVarUserList->GetElementCount();
	for (i=1; i<=dCount; i++)
		{
		(itsDVarUserList->NthElement(i))->
			VariablesRemoved(info.GetFirstIndex(), info.GetCount());
		}

	const JSize fCount = itsFVarUserList->GetElementCount();
	for (i=1; i<=fCount; i++)
		{
		(itsFVarUserList->NthElement(i))->
			VariablesRemoved(info.GetFirstIndex(), info.GetCount());
		}
}

/******************************************************************************
 VariableMoved (private)

 ******************************************************************************/

void
JVariableList::VariableMoved
	(
	const JOrderedSetT::ElementMoved& info
	)
	const
{
JIndex i;

	itsEvalStack->MoveElementToIndex(info.GetOrigIndex(), info.GetNewIndex());

	const JSize dCount = itsDVarUserList->GetElementCount();
	for (i=1; i<=dCount; i++)
		{
		(itsDVarUserList->NthElement(i))->
			VariableMoved(info.GetOrigIndex(), info.GetNewIndex());
		}

	const JSize fCount = itsFVarUserList->GetElementCount();
	for (i=1; i<=fCount; i++)
		{
		(itsFVarUserList->NthElement(i))->
			VariableMoved(info.GetOrigIndex(), info.GetNewIndex());
		}
}

/******************************************************************************
 VariablesSwapped (private)

 ******************************************************************************/

void
JVariableList::VariablesSwapped
	(
	const JOrderedSetT::ElementsSwapped& info
	)
	const
{
JIndex i;

	itsEvalStack->SwapElements(info.GetIndex1(), info.GetIndex2());

	const JSize dCount = itsDVarUserList->GetElementCount();
	for (i=1; i<=dCount; i++)
		{
		(itsDVarUserList->NthElement(i))->
			VariablesSwapped(info.GetIndex1(), info.GetIndex2());
		}

	const JSize fCount = itsFVarUserList->GetElementCount();
	for (i=1; i<=fCount; i++)
		{
		(itsFVarUserList->NthElement(i))->
			VariablesSwapped(info.GetIndex1(), info.GetIndex2());
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JVariableList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	const JCollection* mainList = GetOrderedSet();

	if (sender == const_cast<JCollection*>(mainList) &&
		message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
		assert( info != NULL );
		VariablesInserted(*info);
		}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JOrderedSetT::kElementsRemoved))
		{
		if (!mainList->IsEmpty())
			{
			// AllElementsRemoved should be illegal, but it normally only
			// happens when the object is destructed.  During destruction,
			// we obviously should not complain.

			const JOrderedSetT::ElementsRemoved* info =
				dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
			assert( info != NULL );
			VariablesRemoved(*info);
			}
		}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
		assert( info != NULL );
		VariableMoved(*info);
		}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
		assert( info != NULL );
		VariablesSwapped(*info);
		}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JOrderedSetT::kSorted))
		{
		assert( 0 /* JVariableList doesn't allow elements to be sorted */ );
		}

	JContainer::Receive(sender, message);
}

/******************************************************************************
 VarInserted::AdjustIndex

 ******************************************************************************/

void
JVariableList::VarInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetIndex(), 1, index);
}

/******************************************************************************
 VarRemoved::AdjustIndex

 ******************************************************************************/

JBoolean
JVariableList::VarRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetIndex(), 1, index);
}

/******************************************************************************
 VarMoved::AdjustIndex

 ******************************************************************************/

void
JVariableList::VarMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}
