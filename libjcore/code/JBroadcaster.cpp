/******************************************************************************
 JBroadcaster.cpp

	Mixin class to allow objects to send messages to eachother.  A JBroadcaster
	maintains a list of objects that send it messages (senders) and a list
	of objects that it sends messages to (recipients).

	To send a message, call Broadcast().  The Receive() method is then called
	for each recipient.

	To send a message that requires feedback, call BroadcastWithFeedback().
	The ReceiveWithFeedback() method is then called for each recipient.

	To listen for the death of a sender, override ReceiveGoingAway().
	*** Be sure to read the warnings associated with using this function!

	Because the overhead is so low if there is nobody to broadcast to or
	nobody to listen to, there is no point in making a separate JListener class.

	*** Since this is a mixin class, all classes that wish to inherit from it
		must use "virtual public" inheritance.

	BASE CLASS = none

	Copyright (C) 1994-96 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JBroadcaster.h"
#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JTaskIterator.h"
#include <sstream>
#include <typeinfo>
#include "jx-af/jcore/jAssert.h"

// A derived class is needed because of cross-dependencies between JBroadcaster
// and JCollection.  Typedefs cannot be forward declared in header files, and
// JBroadcasterList must be forward declared.

class JBroadcasterList : public JPtrArray<JBroadcaster>
{
public:

	JBroadcasterList()
		:
		JPtrArray<JBroadcaster>(JPtrArrayT::kForgetAll)
	{ };
};

using JBroadcasterIterator = JTaskIterator<JBroadcaster>;

class JPointerClearList : public JArray<JBroadcaster::ClearPointer>
{
public:

	JPointerClearList() { };
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JBroadcaster::JBroadcaster()
	:
	itsSenders(nullptr),
	itsRecipients(nullptr),
	itsClearPointers(nullptr)
{
}

/******************************************************************************
 Copy constructor (protected)

	Don't copy the connections.  We want this to be identical to the
	default constructor so nobody has to worry about calling us.
	(Otherwise, they would because we are linked in via virtual inheritance.)

 ******************************************************************************/

JBroadcaster::JBroadcaster
	(
	const JBroadcaster& source
	)
{
	itsSenders       = nullptr;
	itsRecipients    = nullptr;
	itsClearPointers = nullptr;
}

/******************************************************************************
 Destructor

	Close all connections.

 ******************************************************************************/

JBroadcaster::~JBroadcaster()
{
	if (itsRecipients != nullptr)
	{
		while (itsRecipients != nullptr && !itsRecipients->IsEmpty())
		{
			JBroadcaster* aRecipient = itsRecipients->GetLastElement();
			itsRecipients->RemoveElement(itsRecipients->GetElementCount());
			aRecipient->RemoveSender(this);
			aRecipient->ClearGone(this);
			aRecipient->ReceiveGoingAway(this);		// do this last in case they die
		}

		jdelete itsRecipients;
	}

	if (itsSenders != nullptr)
	{
		for (auto* sender : *itsSenders)
		{
			sender->RemoveRecipient(this);
		}

		jdelete itsSenders;
	}

	jdelete itsClearPointers;
}

/******************************************************************************
 Assignment operator

	Don't copy the connections.

 ******************************************************************************/

JBroadcaster&
JBroadcaster::operator=
	(
	const JBroadcaster& source
	)
{
	if (this == &source)
	{
		return *this;
	}

	// We don't want to inherit the dependencies, and we don't want to
	// kill the existing ones, so we do nothing.

	return *this;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JBroadcaster::ToString()
	const
{
	std::ostringstream s;
	s << typeid(*this).name() << " (" << this << ")";
	return JString(s.str().c_str());
}

/******************************************************************************
 ListenTo (protected)

	Open a connection by adding the given object to this object's sender list,
	and adding this object to the given object's recipient list.

	If the connection already exists, then we do nothing.

 ******************************************************************************/

void
JBroadcaster::ListenTo
	(
	const JBroadcaster* csender
	)
{
	assert( csender != nullptr );

	auto* sender = const_cast<JBroadcaster*>(csender);
	if (itsSenders == nullptr || !itsSenders->Includes(sender))
	{
		AddSender(sender);
		sender->AddRecipient(this);
	}
}

/******************************************************************************
 StopListening (protected)

	Close a connection by removing the given object from this object's
	sender list, and removing this object from the given objects's
	recipient list.

 ******************************************************************************/

void
JBroadcaster::StopListening
	(
	const JBroadcaster* csender
	)
{
	if (csender == nullptr)
	{
		return;
	}

	auto* sender = const_cast<JBroadcaster*>(csender);
	if (itsSenders != nullptr && itsSenders->Includes(sender))
	{
		RemoveSender(sender);
		sender->RemoveRecipient(this);

		if (itsClearPointers != nullptr)
		{
			const JSize count = itsClearPointers->GetElementCount();
			for (JIndex i=count; i>=1; i--)
			{
				ClearPointer ptr = itsClearPointers->GetElement(i);
				if (ptr.sender == sender)
				{
					itsClearPointers->RemoveElement(i);
				}
			}
		}
	}
}

/******************************************************************************
 Sender and Recipient counts

 ******************************************************************************/

bool
JBroadcaster::HasSenders()
	const
{
	return itsSenders != nullptr && !itsSenders->IsEmpty();
}

JSize
JBroadcaster::GetSenderCount()
	const
{
	if (itsSenders != nullptr)
	{
		return itsSenders->GetElementCount();
	}
	else
	{
		return 0;
	}
}

bool
JBroadcaster::HasRecipients()
	const
{
	return itsRecipients != nullptr && !itsRecipients->IsEmpty();
}

JSize
JBroadcaster::GetRecipientCount()
	const
{
	if (itsRecipients != nullptr)
	{
		return itsRecipients->GetElementCount();
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
 AddRecipient (private)

	Add the given object to the recipient list.  Called by ListenTo().

 ******************************************************************************/

void
JBroadcaster::AddRecipient
	(
	JBroadcaster* recipient
	)
{
	if (itsRecipients == nullptr)
	{
		itsRecipients = jnew JBroadcasterList;
		assert( itsRecipients != nullptr );
	}

	itsRecipients->Prepend(recipient);	// so it gets the -next- message
}

/******************************************************************************
 RemoveRecipient (private)

	Remove the given object from the recipient list. The object will cease
	receiving messages.  Called by StopListening().

 ******************************************************************************/

void
JBroadcaster::RemoveRecipient
	(
	JBroadcaster* recipient
	)
{
	if (itsRecipients != nullptr)
	{
		itsRecipients->Remove(recipient);
		if (itsRecipients->IsEmpty())
		{
			jdelete itsRecipients;
			itsRecipients = nullptr;
		}
	}
}

/******************************************************************************
 AddSender (private)

	Add the given object to the sender list.  Called by ListenTo().

 ******************************************************************************/

void
JBroadcaster::AddSender
	(
	JBroadcaster* sender
	)
{
	if (itsSenders == nullptr)
	{
		itsSenders = jnew JBroadcasterList;
		assert( itsSenders != nullptr );
	}

	itsSenders->Prepend(sender);
}

/******************************************************************************
 RemoveSender (private)

	Remove the given object from the sender list.  Called by StopListening().

 ******************************************************************************/

void
JBroadcaster::RemoveSender
	(
	JBroadcaster* sender
	)
{
	if (itsSenders != nullptr)
	{
		itsSenders->Remove(sender);
		if (itsSenders->IsEmpty())
		{
			jdelete itsSenders;
			itsSenders = nullptr;
		}
	}
}

/******************************************************************************
 Send (protected)

	Send the given message to the specified recipients.  It is the
	responsibility of derived classes to implement a set of useful
	messages. Every message must be derived from JBroadcaster::Message and
	should contain all the information necessary to process the message.

	This function allows messages to be sent to an object in a way that is
	safe but that does not require the C++ type safety mechanism of
	dynamic_cast.  Essentially, this feature is equivalent to the object
	methods provided by Objective C.

 ******************************************************************************/

void
JBroadcaster::Send
	(
	JBroadcaster*	recipient,
	const Message&	message
	)
{
	recipient->Receive(this, message);
}

/******************************************************************************
 BroadcastPrivate (private)

 ******************************************************************************/

void
JBroadcaster::BroadcastPrivate
	(
	const Message& message
	)
{
	assert( itsRecipients != nullptr );

	JBroadcasterIterator	iterator(itsRecipients, kJIteratorStartAtBeginning);
	JBroadcaster*			recipient;

	while (iterator.Next(&recipient))
	{
		recipient->Receive(this, message);
	}
}

/******************************************************************************
 Receive (virtual protected)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

void
JBroadcaster::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
}

/******************************************************************************
 SendWithFeedback (protected)

	Send the given message to the specified recipients.  It is the
	responsibility of derived classes to implement a set of useful
	messages. Every message must be derived from JBroadcaster::Message and
	should contain all the information necessary to process the message.

	This function allows messages to be sent to an object in a way that is
	safe but that does not require the C++ type safety mechanism of
	dynamic_cast.  Essentially, this feature is equivalent to the object
	methods provided by Objective C.

 ******************************************************************************/

void
JBroadcaster::SendWithFeedback
	(
	JBroadcaster*	recipient,
	Message*		message
	)
{
	recipient->ReceiveWithFeedback(this, message);
}

/******************************************************************************
 BroadcastWithFeedbackPrivate (protected)

 ******************************************************************************/

void
JBroadcaster::BroadcastWithFeedbackPrivate
	(
	Message* message
	)
{
	assert( itsRecipients != nullptr );

	JBroadcasterIterator	iterator(itsRecipients, kJIteratorStartAtBeginning);
	JBroadcaster*			recipient;

	while (iterator.Next(&recipient))
	{
		recipient->ReceiveWithFeedback(this, message);
	}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	Process the given message from the given sender and provide feedback via
	the message as appropriate.  This function is not pure virtual because
	not all classes will want to implement it.

 ******************************************************************************/

void
JBroadcaster::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

	Warning:	Since this function may be called from within a -chain-
				of destructors, it is not usually safe to do anything
				inside this function other than directly changing
				instance variables (e.g. setting pointers to nullptr).

	This function is not pure virtual because not all classes will want
	to implement it.

 ******************************************************************************/

void
JBroadcaster::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
}

/******************************************************************************
 ClearWhenGoingAway (protected)

	Register an object that needs to be forgotten when it is deleted.

 ******************************************************************************/

void
JBroadcaster::ClearWhenGoingAway
	(
	const JBroadcaster*	csender,
	void*				pointerToMember
	)
{
	assert( csender != nullptr );

	if (itsClearPointers == nullptr)
	{
		itsClearPointers = jnew JPointerClearList;
		assert( itsClearPointers != nullptr );
	}

	itsClearPointers->AppendElement(
		ClearPointer(const_cast<JBroadcaster*>(csender), pointerToMember));
	ListenTo(csender);
}

/******************************************************************************
 ClearGone (private)

	Remove an object from itsClearPointers.

 ******************************************************************************/

struct PointerSize
{
	int pointerData;
};

void
JBroadcaster::ClearGone
	(
	JBroadcaster* sender
	)
{
	if (itsClearPointers == nullptr)
	{
		return;
	}

	const JSize count = itsClearPointers->GetElementCount();
	for (JIndex i=count; i>=1; i--)
	{
		ClearPointer ptr = itsClearPointers->GetElement(i);
		if (ptr.sender == sender)
		{
			*((PointerSize**) (ptr.pointer)) = nullptr;
			itsClearPointers->RemoveElement(i);
		}
	}
}
