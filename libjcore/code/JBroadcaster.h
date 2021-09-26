/******************************************************************************
 JBroadcaster.h

	Interface for JBroadcaster class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBroadcaster
#define _H_JBroadcaster

#include "jx-af/jcore/JRTTIBase.h"

class JBroadcasterList;
class JPointerClearList;
class JString;

class JBroadcaster
{
public:

	class Message : public JRTTIBase
	{
		public:

			virtual	~Message();

		protected:

			Message(const JUtf8Byte* type)
				:
				JRTTIBase(type)
			{ };
	};

public:

	JBroadcaster();
	JBroadcaster(const JBroadcaster& source);

	virtual	~JBroadcaster();

	const JBroadcaster& operator=(const JBroadcaster& source);

	bool	HasSenders() const;
	JSize	GetSenderCount() const;
	bool	HasRecipients() const;
	JSize	GetRecipientCount() const;

	virtual JString	ToString() const;

protected:

	void	ListenTo(const JBroadcaster* aSender);
	void	StopListening(const JBroadcaster* aSender);
	void	ClearWhenGoingAway(const JBroadcaster* sender, void* pointerToMember);

	void			Send(JBroadcaster* recipient, const Message& message);
	void			Broadcast(const Message& message);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

	void			SendWithFeedback(JBroadcaster* recipient, Message* message);
	void			BroadcastWithFeedback(Message* message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

public:		// ought to be private

	struct ClearPointer
	{
		JBroadcaster*	sender;
		void*			pointer;

		ClearPointer()
			:
			sender(nullptr), pointer(nullptr)
		{ };

		ClearPointer(JBroadcaster* s, void* p)
			:
			sender(s), pointer(p)
		{ };
	};

private:

	JBroadcasterList*	itsSenders;			// the objects to which we listen
	JBroadcasterList*	itsRecipients;		// the objects that listen to us
	JPointerClearList*	itsClearPointers;	// member pointers that need to be cleared

private:

	void	AddRecipient(JBroadcaster* aRecipient);
	void	RemoveRecipient(JBroadcaster* aRecipient);

	void	AddSender(JBroadcaster* aSender);
	void	RemoveSender(JBroadcaster* aSender);

	void	BroadcastPrivate(const Message& message);
	void	BroadcastWithFeedbackPrivate(Message* message);

	void	ClearGone(JBroadcaster* sender);
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
	if (itsRecipients != nullptr)
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
	if (itsRecipients != nullptr)
	{
		BroadcastWithFeedbackPrivate(message);
	}
}

#endif
