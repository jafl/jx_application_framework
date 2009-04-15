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

	Copyright © 1994-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JBroadcaster.h>
#include <JPtrArray.h>
#include <JTaskIterator.h>
#include <jAssert.h>

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

typedef JTaskIterator<JBroadcaster>	JBroadcasterIterator;

/******************************************************************************
 Constructor

 ******************************************************************************/

JBroadcaster::JBroadcaster()
{
	itsSenders    = NULL;
	itsRecipients = NULL;
}

/******************************************************************************
 Copy constructor

	Don't copy the connections.  We want this to be identical to the
	default constructor so nobody has to worry about calling us.
	(Otherwise, they would because we are linked in via virtual inheritance.)

 ******************************************************************************/

JBroadcaster::JBroadcaster
	(
	const JBroadcaster& source
	)
{
	itsSenders    = NULL;
	itsRecipients = NULL;
}

/******************************************************************************
 Destructor

	Close all connections.

 ******************************************************************************/

JBroadcaster::~JBroadcaster()
{
	if (itsRecipients != NULL)
		{
		while (itsRecipients != NULL && !itsRecipients->IsEmpty())
			{
			JBroadcaster* aRecipient = itsRecipients->LastElement();
			itsRecipients->RemoveElement(itsRecipients->GetElementCount());
			aRecipient->RemoveSender(this);
			aRecipient->ReceiveGoingAway(this);		// do this last in case they die
			}

		delete itsRecipients;
		}

	if (itsSenders != NULL)
		{
		const JSize count = itsSenders->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JBroadcaster* aSender = itsSenders->NthElement(i);
			aSender->RemoveRecipient(this);
			}

		delete itsSenders;
		}
}

/******************************************************************************
 Assignment operator

	Don't copy the connections.

 ******************************************************************************/

const JBroadcaster&
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
	assert( csender != NULL );

	JBroadcaster* sender = const_cast<JBroadcaster*>(csender);
	if (itsSenders == NULL || !itsSenders->Includes(sender))
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
	JBroadcaster* sender = const_cast<JBroadcaster*>(csender);
	if (itsSenders != NULL && itsSenders->Includes(sender))
		{
		RemoveSender(sender);
		sender->RemoveRecipient(this);
		}
}

/******************************************************************************
 Sender and Recipient counts

 ******************************************************************************/

JBoolean
JBroadcaster::HasSenders()
	const
{
	return JNegate(itsSenders == NULL || itsSenders->IsEmpty());
}

JSize
JBroadcaster::GetSenderCount()
	const
{
	if (itsSenders != NULL)
		{
		return itsSenders->GetElementCount();
		}
	else
		{
		return 0;
		}
}

JBoolean
JBroadcaster::HasRecipients()
	const
{
	return JNegate(itsRecipients == NULL || itsRecipients->IsEmpty());
}

JSize
JBroadcaster::GetRecipientCount()
	const
{
	if (itsRecipients != NULL)
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
	if (itsRecipients == NULL)
		{
		itsRecipients = new JBroadcasterList;
		assert( itsRecipients != NULL );
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
	if (itsRecipients != NULL)
		{
		itsRecipients->Remove(recipient);
		if (itsRecipients->IsEmpty())
			{
			delete itsRecipients;
			itsRecipients = NULL;
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
	if (itsSenders == NULL)
		{
		itsSenders = new JBroadcasterList;
		assert( itsSenders != NULL );
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
	if (itsSenders != NULL)
		{
		itsSenders->Remove(sender);
		if (itsSenders->IsEmpty())
			{
			delete itsSenders;
			itsSenders = NULL;
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
	assert( itsRecipients != NULL );

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
	assert( itsRecipients != NULL );

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
				instance variables (e.g. setting pointers to NULL).

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
 JBroadcaster::Message

	Refer to the documentation for JRTTIBase for the recommended way to
	define type strings.  Examples of message definitions can be found
	in JArray.h

	BASE CLASS = JRTTIBase

 ******************************************************************************/

/******************************************************************************
 Destructor

 ******************************************************************************/

JBroadcaster::Message::~Message()
{
}

#define JTemplateType JBroadcaster
#include <JPtrArray.tmpls>
#undef JTemplateType
