/******************************************************************************
 JContainer.cpp

							The JContainer Class

	Class for a collection of objects stored in a JOrderedSet.

	By calling InstallOrderedSet in the constructors and copy constructors,
	derived classes need not worry about keeping JCollection::itsElementCount
	up to date.  This class does it automatically by listening to Broadcast
	from the specified JOrderedSet.

	Implementation notes:

	We store a JCollection* only to avoid making this class a template.
	We should actually store an JOrderedSet*.  But instantiating templates
	is such a pain, and, in this case, is conceptually unnecessary.

	BASE CLASS = JCollection

	Copyright © 1994 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JContainer.h>
#include <JOrderedSet.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JContainer::JContainer()
	:
	JCollection()
{
	// We get called before the JOrderedSet gets created.
	// Derived classes must therefore call InstallOrderedSet

	itsOrderedSet = NULL;
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
	// We get called before the JOrderedSet gets created.
	// Derived classes must therefore call InstallOrderedSet()

	itsOrderedSet = NULL;
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

	// Usually, the pointer to the JOrderedSet won't change, so we maintain the
	// element count.  If the pointer does change, the derived class must call
	// InstallOrderedSet() again.

	const JSize origCount = GetElementCount();
	JCollection::operator=(source);
	SetElementCount(origCount);

	return *this;
}

/******************************************************************************
 InstallOrderedSet

 ******************************************************************************/

void
JContainer::InstallOrderedSet
	(
	JCollection* theOrderedSet
	)
{
	if (itsOrderedSet != NULL)
		{
		StopListening(itsOrderedSet);
		}

	itsOrderedSet = theOrderedSet;
	SetElementCount(itsOrderedSet->GetElementCount());
	ListenTo(itsOrderedSet);
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
	if (sender == itsOrderedSet && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast(const JOrderedSetT::ElementsInserted*, &message);
		assert( info != NULL );
		SetElementCount(GetElementCount() + info->GetCount());
		}
	else if (sender == itsOrderedSet && message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast(const JOrderedSetT::ElementsRemoved*, &message);
		assert( info != NULL );
		SetElementCount(GetElementCount() - info->GetCount());
		}
	else
		{
		JCollection::Receive(sender, message);
		}
}
