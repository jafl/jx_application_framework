/******************************************************************************
 Wiz2War.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_Wiz2War
#define _H_Wiz2War

#include "WWSocket.h"			// typedef
#include "wwMessages.h"			// need defintion of WWConnectionErrorType
#include <JXDocumentManager.h>	// need definition of SafetySaveReason
#include <strstream.h>			// template

class JString;
class JXTimerTask;
class WizPlayer;

class Wiz2War : virtual public JBroadcaster
{
public:

	Wiz2War();

	virtual ~Wiz2War();

	JIndex		GetPlayerIndex() const;
	WizPlayer*	GetPlayer(const JIndex index) const;
	JBoolean	GetPlayer(const JIndex index, WizPlayer** player) const;

	void	Connect(const JCharacter* serverAddr, const JCharacter* name);
	void	SendChatMessage(const JIndex receiverIndex, const JCharacter* text);

	void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTimerTask*	itsPingTask;

	// invalid until Connect() is called

	WWSocket*	itsSocket;

	// invalid until ConnectionEstablished

	JIndex					itsPlayerIndex;
	JPtrArray<WizPlayer>*	itsPlayerList;		// item is NULL if player slot is empty

private:

	void	SendMessage(const JIndex id, ostrstream& data);
	void	SendMessage(const JIndex id, const JCharacter* data);

	void	ReceiveMessage();
	void	ReadCurrentState(std::istream& input);

	void	DeleteSocket();

	// not allowed

	Wiz2War(const Wiz2War& source);
	Wiz2War& operator=(const Wiz2War& source);

public:

	// JBroadcaster messages

	static const JCharacter* kConnectionAccepted;
	static const JCharacter* kConnectionRefused;
	static const JCharacter* kDisconnect;

	static const JCharacter* kPlayerJoined;
	static const JCharacter* kPlayerLeft;

	static const JCharacter* kChatMessage;

	// network connection

	class ConnectionAccepted : public JBroadcaster::Message
		{
		public:

			ConnectionAccepted(const JSize maxPlayerCount,
							   const JIndex playerIndex)
				:
				JBroadcaster::Message(kConnectionAccepted),
				itsMaxPlayerCount(maxPlayerCount),
				itsPlayerIndex(playerIndex)
				{ };

			JSize
			GetMaxPlayerCount() const
			{
				return itsMaxPlayerCount;
			};

			JIndex
			GetPlayerIndex() const		// player list not yet created!
			{
				return itsPlayerIndex;
			};

		private:

			const JSize		itsMaxPlayerCount;
			const JIndex	itsPlayerIndex;
		};

	class ConnectionRefused : public JBroadcaster::Message
		{
		public:

			ConnectionRefused(const WWConnectionErrorType err)
				:
				JBroadcaster::Message(kConnectionRefused),
				itsError(err)
				{ };

			WWConnectionErrorType
			GetError() const
			{
				return itsError;
			};

		private:

			const WWConnectionErrorType	itsError;
		};

	class Disconnect : public JBroadcaster::Message
		{
		public:

			Disconnect()
				:
				JBroadcaster::Message(kDisconnect)
				{ };
		};

	// state

	class PlayerJoined : public JBroadcaster::Message
		{
		public:

			PlayerJoined(const JIndex index, const WizPlayer& player,
						 const JBoolean isNew)
				:
				JBroadcaster::Message(kPlayerJoined),
				itsIndex(index), itsPlayer(player), itsIsNewFlag(isNew)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

			const WizPlayer&
			GetPlayer() const
			{
				return itsPlayer;
			};

			JBoolean
			IsNew() const
			{
				return itsIsNewFlag;
			};

		private:

			const JIndex		itsIndex;
			const WizPlayer&	itsPlayer;
			const JBoolean		itsIsNewFlag;
		};

	class PlayerLeft : public JBroadcaster::Message
		{
		public:

			PlayerLeft(const JIndex index, const JBoolean timeout)
				:
				JBroadcaster::Message(kPlayerLeft),
				itsIndex(index), itsWasTimeoutFlag(timeout)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

			JBoolean
			TimedOut() const
			{
				return itsWasTimeoutFlag;
			};

		private:

			const JIndex	itsIndex;
			const JBoolean	itsWasTimeoutFlag;
		};

	// misc

	class ChatMessage : public JBroadcaster::Message
		{
		public:

			ChatMessage(const JIndex senderIndex, const JString& msg)
				:
				JBroadcaster::Message(kChatMessage),
				itsSenderIndex(senderIndex),
				itsMessage(msg)
				{ };

			JIndex
			GetSenderIndex() const
			{
				return itsSenderIndex;
			};

			const JString&
			GetMessage() const
			{
				return itsMessage;
			};

		private:

			const JIndex	itsSenderIndex;
			const JString&	itsMessage;
		};
};


/******************************************************************************
 GetPlayerIndex

 ******************************************************************************/

inline JIndex
Wiz2War::GetPlayerIndex()
	const
{
	return itsPlayerIndex;
}

/******************************************************************************
 GetPlayer

	This does the assert() so you don't have to.

 ******************************************************************************/

inline WizPlayer*
Wiz2War::GetPlayer
	(
	const JIndex index
	)
	const
{
	return itsPlayerList->GetElement(index);
}

#endif
