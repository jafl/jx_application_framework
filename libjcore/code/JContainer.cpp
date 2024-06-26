/******************************************************************************
 JContainer.cpp

							The JContainer Class

	Class for a collection of objects stored in a JList.

	By calling InstallCollection in the constructors and copy constructors,
	derived classes need not worry about keeping JCollection::itsItemCount
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
	// Derived classes must therefore call InstallCollection

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
	// Derived classes must therefore call InstallCollection()

	itsList = nullptr;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

JContainer&
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
	// item count.  If the pointer does change, the derived class must call
	// InstallCollection() again.

	const JSize origCount = GetItemCount();
	JCollection::operator=(source);
	SetItemCount(origCount);

	return *this;
}

/******************************************************************************
 InstallCollection

 ******************************************************************************/

void
JContainer::InstallCollection
	(
	JCollection* list
	)
{
	if (itsList != nullptr)
	{
		StopListening(itsList);
	}

	itsList = list;
	SetItemCount(itsList->GetItemCount());
	ListenTo(itsList);
}

/******************************************************************************
 Receive (virtual protected)

	Keep the number of items up to date.

 ******************************************************************************/

void
JContainer::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsList && message.Is(JListT::kItemsInserted))
	{
		auto& info = dynamic_cast<const JListT::ItemsInserted&>(message);
		SetItemCount(GetItemCount() + info.GetCount());
	}
	else if (sender == itsList && message.Is(JListT::kItemsRemoved))
	{
		auto& info = dynamic_cast<const JListT::ItemsRemoved&>(message);
		SetItemCount(GetItemCount() - info.GetCount());
	}
	else
	{
		JCollection::Receive(sender, message);
	}
}
