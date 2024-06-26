/******************************************************************************
 JXCheckboxGroup.cpp

	This defines the concept of a group of checkboxes that are constrained
	in some way.  Derived classes must implement the actual constraints.

	Derived classes must implement the following function:

		EnforceConstraints
			The value passed in is the index of the checkbox that changed.
			The function should change the state of the checkboxes to
			maintain the constraints.

	BASE CLASS = JContainer

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXCheckboxGroup.h"
#include "JXCheckbox.h"
#include "JXDeleteObjectTask.h"
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCheckboxGroup::JXCheckboxGroup()
	:
	JContainer()
{
	itsCBList = jnew JPtrArray<JXCheckbox>(JPtrArrayT::kForgetAll);
	InstallCollection(itsCBList);
}

JXCheckboxGroup::JXCheckboxGroup
	(
	const JPtrArray<JXCheckbox>& cbList
	)
	:
	JContainer()
{
	itsCBList = jnew JPtrArray<JXCheckbox>(cbList, JPtrArrayT::kForgetAll);
	InstallCollection(itsCBList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCheckboxGroup::~JXCheckboxGroup()
{
	jdelete itsCBList;
}

/******************************************************************************
 Add

 ******************************************************************************/

void
JXCheckboxGroup::Add
	(
	JXCheckbox* cb
	)
{
	if (!itsCBList->Includes(cb))
	{
		itsCBList->Append(cb);
		ListenTo(cb);
	}
}

/******************************************************************************
 Insert

 ******************************************************************************/

void
JXCheckboxGroup::Insert
	(
	const JIndex	index,
	JXCheckbox*		cb
	)
{
	JIndex i;
	if (itsCBList->Find(cb, &i))
	{
		itsCBList->MoveItemToIndex(i, index);
	}
	else
	{
		itsCBList->InsertAtIndex(index, cb);
		ListenTo(cb);
	}
}

/******************************************************************************
 Remove

 ******************************************************************************/

void
JXCheckboxGroup::Remove
	(
	JXCheckbox* cb
	)
{
	StopListening(cb);
	itsCBList->Remove(cb);
}

/******************************************************************************
 AllChecked (protected)

 ******************************************************************************/

bool
JXCheckboxGroup::AllChecked()
	const
{
	return std::all_of(begin(*itsCBList), end(*itsCBList),
			[] (const JXCheckbox* cb) { return cb->IsChecked(); });
}

/******************************************************************************
 AllUnchecked (protected)

 ******************************************************************************/

bool
JXCheckboxGroup::AllUnchecked()
	const
{
	return std::all_of(begin(*itsCBList), end(*itsCBList),
			[] (const JXCheckbox* cb) { return !cb->IsChecked(); });
}

/******************************************************************************
 AllDisabled (protected)

	Returns true if all checkboxes are either hidden or inactive.

 ******************************************************************************/

bool
JXCheckboxGroup::AllDisabled()
	const
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (!CheckboxDisabled(i))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 CheckboxDisabled (protected)

	Returns true if the checkbox is either hidden or inactive.

 ******************************************************************************/

bool
JXCheckboxGroup::CheckboxDisabled
	(
	const JIndex index
	)
	const
{
	JXCheckbox* cb = itsCBList->GetItem(index);
	return !cb->WouldBeVisible() || !cb->WouldBeActive();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXCheckboxGroup::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JXCheckbox::kPushed))
	{
		JIndex i;
		if (FindCheckbox(sender, &i))
		{
			EnforceConstraints(i);
		}
	}

	else
	{
		JContainer::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXCheckboxGroup::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	const JSize cbCount = itsCBList->GetItemCount();
	for (JIndex i=1; i<=cbCount; i++)
	{
		if (itsCBList->GetItem(i) == sender)
		{
			itsCBList->RemoveItem(i);
			break;
		}
	}

	if (itsCBList->IsEmpty())
	{
		JXDeleteObjectTask<JXCheckboxGroup>::Delete(this);
	}

	JContainer::ReceiveGoingAway(sender);
}

/******************************************************************************
 FindCheckbox (protected)

	This takes a JBroadcaster because one is not allowed to downcast from
	a virtual base class, so there is no other way for Receive() to
	obtain a JXCheckbox*.

 ******************************************************************************/

bool
JXCheckboxGroup::FindCheckbox
	(
	JBroadcaster*	obj,
	JIndex*			index
	)
	const
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (itsCBList->GetItem(i) == obj)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}
