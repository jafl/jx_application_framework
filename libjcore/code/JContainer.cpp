/******************************************************************************
 JContainer.cpp

							The JContainer Class

	Class for a collection of objects stored in a JList.

	By calling InstallList in the constructors and copy constructors,
	derived classes need not worry about keeping JCollection::itsElementCount
	up to date.  This class does it automatically by listening to Broadcast
	from the specified JList.

	Implementation notes:

	We store a JCollection* only to avoid making this class a template.
	We should actually store an JList*.  But instantiating templates
	is such a pain, and, in this case, is conceptually unnecessary.

	BASE CLASS = JCollection

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#include "JContainer.h"
#include "JList.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JContainer::JContainer()
	:
	JCollection()
{
	// We get called before the JList gets created.
	// Derived classes must therefore call InstallList

	itsList = nullptr;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JContainer::JContainer
	(
	const JContainer& source
	)
	:
	JCollection(source)
{
	// We get called before the JList gets created.
	// Derived classes must therefore call InstallList()

	itsList = nullptr;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JContainer&
JContainer::operator=
	(
	const JContainer& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	// Usually, the pointer to the JList won't change, so we maintain the
	// element count.  If the pointer does change, the derived class must call
	// InstallList() again.

	const JSize origCount = GetElementCount();
	JCollection::operator=(source);
	SetElementCount(origCount);

	return *this;
}

/******************************************************************************
 InstallList

 ******************************************************************************/

void
JContainer::InstallList
	(
	JCollection* list
	)
{
	if (itsList != nullptr)
		{
		StopListening(itsList);
		}

	itsList = list;
	SetElementCount(itsList->GetElementCount());
	ListenTo(itsList);
}

/******************************************************************************
 Receive (virtual protected)

	Keep the number of elements up to date.

 ******************************************************************************/

void
JContainer::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsList && message.Is(JListT::kElementsInserted))
		{
		const JListT::ElementsInserted* info =
			dynamic_cast<const JListT::ElementsInserted*>(&message);
		assert( info != nullptr );
		SetElementCount(GetElementCount() + info->GetCount());
		}
	else if (sender == itsList && message.Is(JListT::kElementsRemoved))
		{
		const JListT::ElementsRemoved* info =
			dynamic_cast<const JListT::ElementsRemoved*>(&message);
		assert( info != nullptr );
		SetElementCount(GetElementCount() - info->GetCount());
		}
	else
		{
		JCollection::Receive(sender, message);
		}
}
