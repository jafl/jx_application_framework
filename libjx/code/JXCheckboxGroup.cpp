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
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCheckboxGroup::JXCheckboxGroup()
	:
	JContainer()
{
	itsCBList = jnew JPtrArray<JXCheckbox>(JPtrArrayT::kForgetAll);
	assert( itsCBList != nullptr );

	InstallList(itsCBList);
}

JXCheckboxGroup::JXCheckboxGroup
	(
	const JPtrArray<JXCheckbox>& cbList
	)
	:
	JContainer()
{
	itsCBList = jnew JPtrArray<JXCheckbox>(cbList, JPtrArrayT::kForgetAll);
	assert( itsCBList != nullptr );

	InstallList(itsCBList);
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
		itsCBList->MoveElementToIndex(i, index);
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

JBoolean
JXCheckboxGroup::AllChecked()
	const
{
	return JI2B(
		std::all_of(begin(*itsCBList), end(*itsCBList),
			[] (const JXCheckbox* cb) { return cb->IsChecked(); }));
}

/******************************************************************************
 AllUnchecked (protected)

 ******************************************************************************/

JBoolean
JXCheckboxGroup::AllUnchecked()
	const
{
	return JI2B(
		std::all_of(begin(*itsCBList), end(*itsCBList),
			[] (const JXCheckbox* cb) { return !cb->IsChecked(); }));
}

/******************************************************************************
 AllDisabled (protected)

	Returns kJTrue if all checkboxes are either hidden or inactive.

 ******************************************************************************/

JBoolean
JXCheckboxGroup::AllDisabled()
	const
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (!CheckboxDisabled(i))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 CheckboxDisabled (protected)

	Returns kJTrue if the checkbox is either hidden or inactive.

 ******************************************************************************/

JBoolean
JXCheckboxGroup::CheckboxDisabled
	(
	const JIndex index
	)
	const
{
	JXCheckbox* cb = itsCBList->GetElement(index);
	return JI2B(!cb->WouldBeVisible() || !cb->WouldBeActive());
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
	const JSize cbCount = itsCBList->GetElementCount();
	for (JIndex i=1; i<=cbCount; i++)
		{
		if (itsCBList->GetElement(i) == sender)
			{
			itsCBList->RemoveElement(i);
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

JBoolean
JXCheckboxGroup::FindCheckbox
	(
	JBroadcaster*	obj,
	JIndex*			index
	)
	const
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (itsCBList->GetElement(i) == obj)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}
