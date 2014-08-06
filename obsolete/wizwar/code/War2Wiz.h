/******************************************************************************
 War2Wiz.h

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_War2Wiz
#define _H_War2Wiz

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#include <strstream.h>		// template

class JString;
class WarPlayer;
class WarSocket;

class War2Wiz : virtual public JBroadcaster
{
public:

	War2Wiz();

	virtual ~War2Wiz();

	void	ConnectionEstablished(WarSocket* socket);
	void	SendChatMessage(const JIndex senderIndex, const JIndex receiverIndex,
							const JCharacter* text);

	JBoolean	PlayerIndexValid(const JIndex index) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	JPtrArray<WarPlayer>*	itsPlayerList;		// item is NULL if player slot is empy
	WarSocket*				itsLastRecipient;	// NULL unless sending

private:

	void	SendMessage(const JIndex playerIndex, const JIndex id, ostrstream& data);
	void	SendMessage(const JIndex playerIndex, const JIndex id, const JCharacter* data);
	void	CheckForDeadConnection(WarSocket* socket);

	void	ReceiveMessage(const JIndex clientIndex, WarSocket& socket);
	void	ReceiveDisconnect(JBroadcaster* sender, const JBoolean timeout);

	void		ReceiveClientIdentity(const JIndex senderIndex, WarPlayer* sender, istream& input);
	JBoolean	PlayerNameUsed(const JIndex senderIndex, const WarPlayer& sender) const;
	void		DeletePlayer(const JIndex index);
	void		WriteCurrentState(ostream& data, const JIndex excludeIndex) const;

	WarSocket*	GetSender(JBroadcaster* sender, JIndex* senderIndex) const;
	JBoolean	GetSocket(const JIndex index, WarSocket** socket) const;

	// not allowed

	War2Wiz(const War2Wiz& source);
	War2Wiz& operator=(const War2Wiz& source);
};


/******************************************************************************
 PlayerIndexValid

 ******************************************************************************/

inline JBoolean
War2Wiz::PlayerIndexValid
	(
	const JIndex index
	)
	const
{
	return JI2B(itsPlayerList->IndexValid(index) &&
				itsPlayerList->NthElement(index) != NULL);
}

#endif
