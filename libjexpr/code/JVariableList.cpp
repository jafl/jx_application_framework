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
	false.  Others may choose to store a mixture of JFloat and JComplex.
	In some applications, one may even store only JFloat because one never uses
	JComplex at all.

	By providing functions for keeping an evalutation stack, we allow derived
	classes to catch variables defined in terms of themselves and to avoid the
	resulting infinite recursion that GetNumericValue() would get stuck in.
	The stack is implemented as an array of bool so all operations are
	merely array look-ups.

	BASE CLASS = JContainer

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JVariableList.h"
#include "JNamedConstant.h"
#include "JUserInputFunction.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JVariableList::kVarInserted          = "VarInserted::JVariableList";
const JUtf8Byte* JVariableList::kVarRemoved           = "VarRemoved::JVariableList";
const JUtf8Byte* JVariableList::kVarMoved             = "VarMoved::JVariableList";
const JUtf8Byte* JVariableList::kVarNameChanged       = "VarNameChanged::JVariableList";
const JUtf8Byte* JVariableList::kVarValueChanged      = "VarValueChanged::JVariableList";
const JUtf8Byte* JVariableList::kDiscValueNameChanged = "DiscValueNameChanged::JVariableList";

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
	jdelete itsVarUserList;		// objects deleted by owners
	jdelete itsEvalStack;
}

/******************************************************************************
 JVariableListX

 ******************************************************************************/

void
JVariableList::JVariableListX()
{
	itsVarUserList = jnew JPtrArray<JFunction>(JPtrArrayT::kForgetAll);
	itsEvalStack   = jnew JArray<bool>(10);
}

/******************************************************************************
 ParseVariableName

	Returns true if expr is the name of a variable.

 ******************************************************************************/

bool
JVariableList::ParseVariableName
	(
	const JString&	name,
	JIndex*			index
	)
	const
{
	const JString* n   = &name;
	bool allocated = false;
	if (n->Contains(JUserInputFunction::kSwitchFontCharacter))
	{
		n = jnew JString(JUserInputFunction::ConvertToGreek(name));
		allocated = true;
	}

	bool found = false;

	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (*n == GetVariableName(i))
		{
			*index = i;
			found  = true;
			break;
		}
	}

	if (allocated)
	{
		jdelete n;
	}

	return found;
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

	itsEvalStack->SetItem(variableIndex, true);
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

	itsEvalStack->SetItem(variableIndex, false);
}

/******************************************************************************
 NameValid (static)

	Keep in sync with JExprScannerL.l

 ******************************************************************************/

static const JRegex namePattern("^(\\p{L}|`)(\\p{L}|\\d|`|_)*$");

bool
JVariableList::NameValid
	(
	const JString& name
	)
{
	return (namePattern.Match(name) &&
			!JNamedConstant::IsNamedConstant(name));
}

/******************************************************************************
 OKToRemoveVariable

	Never delete a variable from the list without checking this!

 ******************************************************************************/

bool
JVariableList::OKToRemoveVariable
	(
	const JIndex variableIndex
	)
	const
{
	const JSize count = itsVarUserList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (itsVarUserList->GetItem(i)->UsesVariable(variableIndex))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 Reference counting

	These functions are const because the information isn't really
	part of the object.  It's just machinery behind the scenes.

 ******************************************************************************/

void
JVariableList::VariableUserCreated
	(
	JFunction* f
	)
	const
{
	itsVarUserList->Append(f);
}

void
JVariableList::VariableUserDeleted
	(
	JFunction* f
	)
	const
{
	itsVarUserList->Remove(f);
}

/******************************************************************************
 VariablesInserted (private)

 ******************************************************************************/

void
JVariableList::VariablesInserted
	(
	const JListT::ItemsInserted& info
	)
	const
{
	for (JIndex i=1; i<=info.GetCount(); i++)
	{
		itsEvalStack->InsertItemAtIndex(info.GetFirstIndex(), false);
	}

	const JSize count = itsVarUserList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		(itsVarUserList->GetItem(i))->
			VariablesInserted(info.GetFirstIndex(), info.GetCount());
	}
}

/******************************************************************************
 VariablesRemoved (private)

 ******************************************************************************/

void
JVariableList::VariablesRemoved
	(
	const JListT::ItemsRemoved& info
	)
	const
{
	#ifndef NDEBUG
{
	for (JIndex i=info.GetFirstIndex(); i<=info.GetLastIndex(); i++)
	{
		assert( OKToRemoveVariable(i) );
	}
}
	#endif

	itsEvalStack->RemoveNextItems(info.GetFirstIndex(), info.GetCount());

	const JSize count = itsVarUserList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		(itsVarUserList->GetItem(i))->
			VariablesRemoved(info.GetFirstIndex(), info.GetCount());
	}
}

/******************************************************************************
 VariableMoved (private)

 ******************************************************************************/

void
JVariableList::VariableMoved
	(
	const JListT::ItemMoved& info
	)
	const
{
	itsEvalStack->MoveItemToIndex(info.GetOrigIndex(), info.GetNewIndex());

	const JSize count = itsVarUserList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		(itsVarUserList->GetItem(i))->
			VariableMoved(info.GetOrigIndex(), info.GetNewIndex());
	}
}

/******************************************************************************
 VariablesSwapped (private)

 ******************************************************************************/

void
JVariableList::VariablesSwapped
	(
	const JListT::ItemsSwapped& info
	)
	const
{
	itsEvalStack->SwapItems(info.GetIndex1(), info.GetIndex2());

	const JSize count = itsVarUserList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		(itsVarUserList->GetItem(i))->
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
	const JCollection* mainList = GetList();

	if (sender == const_cast<JCollection*>(mainList) &&
		message.Is(JListT::kItemsInserted))
	{
		auto& info = dynamic_cast<const JListT::ItemsInserted&>(message);
		VariablesInserted(info);
	}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JListT::kItemsRemoved))
	{
		if (!mainList->IsEmpty())
		{
			// AllItemsRemoved should be illegal, but it normally only
			// happens when the object is destructed.  During destruction,
			// we obviously should not complain.

			auto& info = dynamic_cast<const JListT::ItemsRemoved&>(message);
			VariablesRemoved(info);
		}
	}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JListT::kItemMoved))
	{
		auto& info = dynamic_cast<const JListT::ItemMoved&>(message);
		VariableMoved(info);
	}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JListT::kItemsSwapped))
	{
		auto& info = dynamic_cast<const JListT::ItemsSwapped&>(message);
		VariablesSwapped(info);
	}
	else if (sender == const_cast<JCollection*>(mainList) &&
			 message.Is(JListT::kSorted))
	{
		assert_msg( 0, "JVariableList doesn't allow elements to be sorted" );
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

bool
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
