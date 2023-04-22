#ifndef _T_JBroadcasterMessageMap
#define _T_JBroadcasterMessageMap

/******************************************************************************
 JBroadcasterMessageMap.tmpl

	BASE CLASS = JHashTable<JBroadcasterMessageTarget>

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "JBroadcasterMessageMap.h"
#include "JBroadcasterMessageTarget.h"
#include "JBroadcaster.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 *****************************************************************************/

JBroadcasterMessageMap::JBroadcasterMessageMap()
{
	JHashTable<JBroadcasterMessageTarget>::DisallowCursors();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JBroadcasterMessageMap::~JBroadcasterMessageMap()
{
	RemoveAll();
}

/******************************************************************************
 Contains

	*** This code is also used to position the cursor for use by other
		functions in both this class and derived classes!

 *****************************************************************************/

bool
JBroadcasterMessageMap::Contains
	(
	const std::type_info& key
	)
	const
{
	auto* cursor = JHashTable<JBroadcasterMessageTarget>::GetCursor();
	JBroadcasterMessageTarget hashEntry(key);
	cursor->ResetKey(hashEntry);
	return cursor->NextKey();
}

/******************************************************************************
 GetElement

	If the element is found, sets value to it and returns true; otherwise,
	nulls value and returns false.

 *****************************************************************************/

bool
JBroadcasterMessageMap::GetElement
	(
	const std::type_info&	key,
	const JBroadcaster*		obj,
	boost::any**			f
	)
	const
{
	if (Contains(key))
	{
		auto v = JHashTable<JBroadcasterMessageTarget>::GetCursor()->GetValue();
		for (const auto t : *v.list)
		{
			if (t.obj == obj)
			{
				*f = t.f;
				return true;
			}
		}
	}

	*f = nullptr;
	return false;
}

/******************************************************************************
 GetList

	If the list is found, sets list to it and returns true; otherwise,
	nulls list and returns false.

 *****************************************************************************/

bool
JBroadcasterMessageMap::GetList
	(
	const std::type_info&				key,
	JArray<JBroadcasterMessageTuple>**	list
	)
	const
{
	if (Contains(key))
	{
		auto v = JHashTable<JBroadcasterMessageTarget>::GetCursor()->GetValue();
		*list  = v.list;
		return true;
	}

	*list = nullptr;
	return false;
}

/******************************************************************************
 SetElement

	The element is set if it already existed and 'type' is kIfOld or kAlways
	(the default) or if it did not exist and 'type' is kIfNew or kAlways.

	Returns true if the element was set, false otherwise.
	*existed is set to true if there was something already there.

	We prepend new items so they get the -next- message.

 *****************************************************************************/

void
JBroadcasterMessageMap::SetElement
	(
	const std::type_info&	key,
	JBroadcaster*			obj,
	boost::any*				f
	)
{
	auto* cursor = JHashTable<JBroadcasterMessageTarget>::GetCursor();
	JBroadcasterMessageTarget hashEntry(key);
	cursor->ResetKey(hashEntry);
	cursor->ForceNextMapInsertKey();

	if (cursor->IsFull())
	{
		auto* list = cursor->GetValue().list;

		bool found = false;

		const JSize count = list->GetElementCount();
		for (JIndex i=1; i<count; i++)
		{
			auto t = list->GetElement(i);
			if (t.obj == obj)
			{
				t.f = f;
				list->SetElement(i, t);
				found = true;
				break;
			}
		}

		if (!found)
		{
			list->PrependElement(JBroadcasterMessageTuple(obj, f));
		}
	}
	else
	{
		hashEntry.list = jnew JArray<JBroadcasterMessageTuple>;
		assert( hashEntry.list != nullptr );

		hashEntry.list->PrependElement(JBroadcasterMessageTuple(obj, f));

		cursor->Set(cursor->GetCursorHashValue(), hashEntry);
	}
}

/******************************************************************************
 RemoveTuple

 *****************************************************************************/

void
JBroadcasterMessageMap::RemoveTuple
	(
	const std::type_info&	key,
	const JBroadcaster*		obj
	)
{
	if (!Contains(key))
	{
		return;
	}

	auto* cursor = JHashTable<JBroadcasterMessageTarget>::GetCursor();
	auto* list   = cursor->GetValue().list;

	const JSize count = list->GetElementCount();
	for (JIndex i=1; i<count; i++)
	{
		auto t = list->GetElement(i);
		if (t.obj == obj)
		{
			t.CleanOut();
			list->RemoveElement(i);

			if (list->IsEmpty())
			{
				RemoveType(key);
			}
			break;
		}
	}
}

/******************************************************************************
 RemoveType

 *****************************************************************************/

void
JBroadcasterMessageMap::RemoveType
	(
	const std::type_info& key
	)
{
	if (Contains(key))
	{
		auto* cursor = JHashTable<JBroadcasterMessageTarget>::GetCursor();
		const_cast<JBroadcasterMessageTarget&>(cursor->GetValue()).CleanOut();
		cursor->Remove();
	}
}

/******************************************************************************
 RemoveAll

 *****************************************************************************/

void
JBroadcasterMessageMap::RemoveAll()
{
	if (!JHashTable<JBroadcasterMessageTarget>::IsEmpty())
	{
		auto* cursor = JHashTable<JBroadcasterMessageTarget>::GetCursor();
		cursor->Reset();
		while (cursor->NextFull())
		{
			// delete, but don't mark empty to avoid multiple resizing
			const_cast<JBroadcasterMessageTarget&>(cursor->GetValue()).CleanOut();
		}
	}

	// Mark all at once and possibly resize; we want to call even if the
	// hash table is already empty to zero out the load count
	JHashTable<JBroadcasterMessageTarget>::MarkAllEmpty();
}

/******************************************************************************
 JBroadcasterMessageTarget::CleanOut

 *****************************************************************************/

void
JBroadcasterMessageTarget::CleanOut()
{
	for (auto item : *list)
	{
		item.CleanOut();
	}
	jdelete list;
}

/******************************************************************************
 JBroadcasterMessageTuple::CleanOut

	Not inline to avoid circular dependencies

 *****************************************************************************/

void
JBroadcasterMessageTuple::CleanOut()
{
	jdelete f;
}

#endif
