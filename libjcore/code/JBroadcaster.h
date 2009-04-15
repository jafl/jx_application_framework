/******************************************************************************
 JBroadcaster.h

	Interface for JBroadcaster class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBroadcaster
#define _H_JBroadcaster

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JRTTIBase.h>

class JBroadcasterList;

class JBroadcaster
{
public:

	class Message : public JRTTIBase
		{
		public:

			virtual	~Message();

		protected:

			Message(const JCharacter* type)
				:
				JRTTIBase(type)
			{ };
		};

public:

	JBroadcaster();
	JBroadcaster(const JBroadcaster& source);

	virtual	~JBroadcaster();

	const JBroadcaster& operator=(const JBroadcaster& source);

	JBoolean	HasSenders() const;
	JSize		GetSenderCount() const;
	JBoolean	HasRecipients() const;
	JSize		GetRecipientCount() const;

protected:

	void	ListenTo(const JBroadcaster* aSender);
	void	StopListening(const JBroadcaster* aSender);

	void			Send(JBroadcaster* recipient, const Message& message);
	void			Broadcast(const Message& message);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

	void			SendWithFeedback(JBroadcaster* recipient, Message* message);
	void			BroadcastWithFeedback(Message* message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JBroadcasterList*	itsSenders;			// the objects to which we listen
	JBroadcasterList*	itsRecipients;		// the objects that listen to us

private:

	void	AddRecipient(JBroadcaster* aRecipient);
	void	RemoveRecipient(JBroadcaster* aRecipient);

	void	AddSender(JBroadcaster* aSender);
	void	RemoveSender(JBroadcaster* aSender);

	void	BroadcastPrivate(const Message& message);
	void	BroadcastWithFeedbackPrivate(Message* message);
};


/******************************************************************************
 Broadcast (protected)

	Send the given message to all recipients.  It is the responsibility
	of derived classes to implement a set of useful messages. Every
	message must be derived from JBroadcaster::Message and should contain
	all the information necessary to process the message.

	We use an iterator because anything could happen while calling Receive().

	By inlining this part of the function, we avoid the overhead of a
	function call unless somebody is actually listening.

 ******************************************************************************/

inline void
JBroadcaster::Broadcast
	(
	const Message& message
	)
{
	if (itsRecipients != NULL)
		{
		BroadcastPrivate(message);
		}
}

/******************************************************************************
 BroadcastWithFeedback (protected)

	Send the given message to all recipients and expect a reply.  The reply
	is message dependent and therefore stored in the message, so we simply
	send the message as a non-const object and let the receiver who understands
	the message deal with it.

	We use an iterator because anything could happen while calling
	ReceiveWithFeedback().

	By inlining this part of the function, we avoid the overhead of a
	function call unless somebody is actually listening.

 ******************************************************************************/

inline void
JBroadcaster::BroadcastWithFeedback
	(
	Message* message
	)
{
	if (itsRecipients != NULL)
		{
		BroadcastWithFeedbackPrivate(message);
		}
}

#endif
