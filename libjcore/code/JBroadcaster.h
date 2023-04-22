/******************************************************************************
 JBroadcaster.h

	Interface for JBroadcaster class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBroadcaster
#define _H_JBroadcaster

#include "JRTTIBase.h"

class JBroadcasterMessageMap;
class JString;
template <class T> class JArray;
template <class T> class JPtrArray;

class JBroadcaster
{
public:

	class Message : public JRTTIBase
	{
		protected:

			using JRTTIBase::JRTTIBase;
	};

public:

	JBroadcaster();

	virtual ~JBroadcaster();

	JBroadcaster& operator=(const JBroadcaster& source);

	bool	HasSenders() const;
	JSize	GetSenderCount() const;
	bool	HasRecipients() const;
	JSize	GetRecipientCount() const;

	virtual JString	ToString() const;

	// public so objects can standardize attaching handlers to themselves

	template <class T>
	void	ListenTo(const JBroadcaster* sender,
					 const std::function<void(const T&)>& f);

protected:

	JBroadcaster(const JBroadcaster& source);

	void	ListenTo(const JBroadcaster* sender);
	void	StopListening(const JBroadcaster* sender);
	void	ClearWhenGoingAway(const JBroadcaster* sender, void* pointerToMember);

	void	StopListening(const JBroadcaster* sender, const std::type_info& messageType);

	template <class T>
	void	Send(JBroadcaster* recipient, const T& message);
	template <class T>
	void	Broadcast(const T& message);

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

	JPtrArray<JBroadcaster>*	itsSenders;			// the objects to which we listen
	JPtrArray<JBroadcaster>*	itsRecipients;		// the objects that listen to us
	JArray<JBroadcaster::ClearPointer>*	itsClearPointers;	// member pointers that need to be cleared

	JBroadcasterMessageMap*	itsCallSources;
	JBroadcasterMessageMap*	itsCallTargets;

private:

	void	AddRecipient(JBroadcaster* recipient);
	void	RemoveRecipient(JBroadcaster* recipient);

	void	AddSender(JBroadcaster* sender);
	void	RemoveSender(JBroadcaster* sender);

	template <class T>
	void	AddCallTarget(JBroadcaster* recipient, const std::function<void(const T&)>& f);
	void	RemoveCallTarget(JBroadcaster* recipient, const std::type_info& messageType);

	void	AddCallSource(JBroadcaster* sender, const std::type_info& messageType);
	void	RemoveCallSource(JBroadcaster* sender, const std::type_info& messageType);

	template <class T>
	void	BroadcastPrivate(const T& message);
	void	BroadcastWithFeedbackPrivate(Message* message);

	void	ClearGone(JBroadcaster* sender);
};


/******************************************************************************
 Broadcast (protected)

	Send the given message to all recipients.  It is the responsibility
	of derived classes to implement a set of useful messages. Every
	message must be derived from JBroadcaster::Message and should contain
	all the information necessary to process the message.

	By inlining this part of the function, we avoid the overhead of a
	function call unless somebody is actually listening.

 ******************************************************************************/

template <class T>
inline void
JBroadcaster::Broadcast
	(
	const T& message
	)
{
	if (itsRecipients != nullptr || itsCallTargets != nullptr)
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
