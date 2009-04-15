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

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCheckboxGroup.h>
#include <JXCheckbox.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCheckboxGroup::JXCheckboxGroup()
	:
	JContainer()
{
	itsCBList = new JPtrArray<JXCheckbox>(JPtrArrayT::kForgetAll);
	assert( itsCBList != NULL );

	InstallOrderedSet(itsCBList);
}

JXCheckboxGroup::JXCheckboxGroup
	(
	const JPtrArray<JXCheckbox>& cbList
	)
	:
	JContainer()
{
	itsCBList = new JPtrArray<JXCheckbox>(cbList, JPtrArrayT::kForgetAll, kJFalse);
	assert( itsCBList != NULL );

	InstallOrderedSet(itsCBList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCheckboxGroup::~JXCheckboxGroup()
{
	delete itsCBList;
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
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (!(itsCBList->NthElement(i))->IsChecked())
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 AllUnchecked (protected)

 ******************************************************************************/

JBoolean
JXCheckboxGroup::AllUnchecked()
	const
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if ((itsCBList->NthElement(i))->IsChecked())
			{
			return kJFalse;
			}
		}

	return kJTrue;
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
	JXCheckbox* cb = itsCBList->NthElement(index);
	return JConvertToBoolean(!cb->WouldBeVisible() || !cb->WouldBeActive());
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
		if (itsCBList->NthElement(i) == obj)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}
