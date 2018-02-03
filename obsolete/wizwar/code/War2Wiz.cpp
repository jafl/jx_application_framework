/******************************************************************************
 War2Wiz.cpp

	Maintains a connection to the Wiz2War object in a client.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "War2Wiz.h"
#include "WarPlayer.h"
#include "WarSocket.h"
#include "warGlobals.h"
#include "wwMessages.h"
#include "wwUtil.h"
#include "wwConstants.h"
#include <JThisProcess.h>
#include <jStrStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

War2Wiz::War2Wiz()
{
	itsPlayerList = new JPtrArray<WarPlayer>(JPtrArrayT::kDeleteAll);
	assert( itsPlayerList != NULL );

	itsLastRecipient = NULL;
	ListenTo(JThisProcess::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

War2Wiz::~War2Wiz()
{
	delete itsPlayerList;
}

/******************************************************************************
 ConnectionEstablished

	This is called when the Wiz2War object connects from a client.

 ******************************************************************************/

void
War2Wiz::ConnectionEstablished
	(
	WarSocket* socket
	)
{
	JIndex playerIndex = 0;

	const JSize count = itsPlayerList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (itsPlayerList->GetElement(i) == NULL)
			{
			playerIndex = i;
			break;
			}
		}

	WarPlayer* player = new WarPlayer(
		(playerIndex == 0 ? itsPlayerList->GetElementCount()+1 : playerIndex),
		socket);
	assert( player != NULL );

	if (playerIndex > 0)
		{
		itsPlayerList->SetElement(playerIndex, player, JPtrArrayT::kDelete);
		}
	else
		{
		itsPlayerList->Append(player);
		}

	WWSetProtocol(socket);
	ListenTo(socket);

	// wait for kWWClientProtocolVersion
}

/******************************************************************************
 SendChatMessage

 ******************************************************************************/

void
War2Wiz::SendChatMessage
	(
	const JIndex		senderIndex,
	const JIndex		receiverIndex,
	const JCharacter*	text
	)
{
	assert( PlayerIndexValid(senderIndex) );

	ostrstream data;
	data << senderIndex;
	data << ' ' << JString(text);
	SendMessage(receiverIndex, kWWChatMessage, data);
}

/******************************************************************************
 SendMessage (protected)

 ******************************************************************************/

void
War2Wiz::SendMessage
	(
	const JIndex	playerIndex,
	const JIndex	id,
	ostrstream&		data
	)
{
	data << ends;
	SendMessage(playerIndex, id, data.str());
	JUnfreeze(data);
}

void
War2Wiz::SendMessage
	(
	const JIndex		playerIndex,
	const JIndex		id,
	const JCharacter*	data
	)
{
	JString msg(id, 0);
	msg += " ";
	msg += data;

	if (playerIndex == kWWAllPlayersIndex)
		{
		const JSize playerCount = itsPlayerList->GetElementCount();
		for (JIndex i=playerCount; i>=1; i--)
			{
			WarSocket* socket;
			if (GetSocket(i, &socket))
				{
				socket->SendMessage(msg);
				CheckForDeadConnection(socket);		// can delete socket
				}
			}
		}
	else
		{
		WarSocket* socket;
		const JBoolean open = GetSocket(playerIndex, &socket);
		assert( open );
		socket->SendMessage(msg);
		CheckForDeadConnection(socket);
		}
}

/******************************************************************************
 GetSocket (private)

 ******************************************************************************/

JBoolean
War2Wiz::GetSocket
	(
	const JIndex	index,
	WarSocket**		socket
	)
	const
{
	const WarPlayer* player = itsPlayerList->GetElement(index);
	if (player != NULL)
		{
		*socket = player->GetSocket();
		return kJTrue;
		}
	else
		{
		*socket = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 CheckForDeadConnection (private)

 ******************************************************************************/

void
War2Wiz::CheckForDeadConnection
	(
	WarSocket* socket
	)
{
	itsLastRecipient = socket;
	JThisProcess::CheckForSignals();
	itsLastRecipient = NULL;
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	Listen for SIGPIPE.

 ******************************************************************************/

void
War2Wiz::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == JThisProcess::Instance() && message->Is(JThisProcess::kBrokenPipe))
		{
		JThisProcess::Signal* info = dynamic_cast<JThisProcess::Signal*>(message);
		assert( info != NULL );
		info->SetCaught();

// This kills *all* the connections, possibly because signals arrive asynch.
//		ReceiveDisconnect(itsLastRecipient, kJTrue);
		}
	else
		{
		JBroadcaster::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
War2Wiz::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JMessageProtocolT::kMessageReady))
		{
		JIndex senderIndex;
		WarSocket* socket = GetSender(sender, &senderIndex);
		ReceiveMessage(senderIndex, *socket);
		}

	else if (message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
		ReceiveDisconnect(sender, kJFalse);
		}
	else if (message.Is(WarSocket::kTimeout))
		{
		ReceiveDisconnect(sender, kJTrue);
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 GetSender (private)

	We return a WarSocket* because we can't downcast from JBroadcaster.

 ******************************************************************************/

WarSocket*
War2Wiz::GetSender
	(
	JBroadcaster*	sender,
	JIndex*			senderIndex
	)
	const
{
	const JSize count = itsPlayerList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const WarPlayer* player = itsPlayerList->GetElement(i);
		if (player != NULL && player->GetSocket() == sender)
			{
			*senderIndex = i;
			return player->GetSocket();
			}
		}

	assert( 0 /* War2Wiz received message from unknown socket */ );
	return NULL;
}

/******************************************************************************
 ReceiveDisconnect (private)

 ******************************************************************************/

void
War2Wiz::ReceiveDisconnect
	(
	JBroadcaster*	sender,
	const JBoolean	timeout
	)
{
	JIndex senderIndex;
	GetSender(sender, &senderIndex);
	DeletePlayer(senderIndex);

	ostrstream data;
	data << senderIndex << ' ' << timeout;
	SendMessage(kWWAllPlayersIndex, kWWPlayerLeft, data);
}

/******************************************************************************
 ReceiveMessage (private)

	Decode message from a client.

 ******************************************************************************/

void
War2Wiz::ReceiveMessage
	(
	const JIndex	senderIndex,
	WarSocket&		socket
	)
{
	WarPlayer* sender = itsPlayerList->GetElement(senderIndex);

	JString s;
	const JBoolean ok = socket.GetNextMessage(&s);
	assert( ok );

	jistrstream(input, s.GetCString(), s.GetLength());
	WWMessageType type;
	input >> type;

	if (type == kWWClientIdentity)
		{
		ReceiveClientIdentity(senderIndex, sender, input);
		}

	else if (type == kWWClientAlive)
		{
		socket.ResetTimer();
		}

	else if (type == kWWChatMessage)
		{
		JIndex receiverIndex;
		JString msg;
		input >> receiverIndex >> msg;
		if (receiverIndex == kWWAllPlayersIndex || PlayerIndexValid(receiverIndex))
			{
			SendChatMessage(senderIndex, receiverIndex, msg);
			}
		}
}

/******************************************************************************
 ReceiveClientIdentity (private)

	Decode kWWClientIdentity from a client.

 ******************************************************************************/

void
War2Wiz::ReceiveClientIdentity
	(
	const JIndex	senderIndex,
	WarPlayer*		sender,
	std::istream&		input
	)
{
	JFileVersion vers;
	input >> vers;

	JBoolean error = kJTrue;

	ostrstream data;
	if (vers != kWWCurrentProtocolVersion)
		{
		data << kJFalse << ' ' << kWWWrongProtocolVersion;
		}
	else if (senderIndex > (WarGetSetup())->GetMaxPlayerCount())
		{
		data << kJFalse << ' ' << kWWGameFull;
		}
	else
		{
		sender->ReadIdentity(input);
		if (input.fail())
			{
			data << kJFalse << ' ' << kWWBadClient;
			}
		else if (PlayerNameUsed(senderIndex, *sender))
			{
			data << kJFalse << ' ' << kWWPlayerNameUsed;
			}
		else
			{
			error = kJFalse;
			data << kJTrue;
			data << ' ' << (WarGetSetup())->GetMaxPlayerCount();
			data << ' ' << senderIndex << ' ';
			WriteCurrentState(data, senderIndex);
			}
		}
	SendMessage(senderIndex, kWWConnectionEstablished, data);

	if (error)
		{
		DeletePlayer(senderIndex);
		}
	else
		{
		ostrstream data;
		data << senderIndex;
		data << ' ';
		sender->WriteState(data);
		SendMessage(kWWAllPlayersIndex, kWWPlayerJoined, data);
		}
}

/******************************************************************************
 PlayerNameUsed (private)

 ******************************************************************************/

JBoolean
War2Wiz::PlayerNameUsed
	(
	const JIndex		senderIndex,
	const WarPlayer&	sender
	)
	const
{
	const JSize playerCount = itsPlayerList->GetElementCount();
	for (JIndex i=1; i<=playerCount; i++)
		{
		const WarPlayer* player = itsPlayerList->GetElement(i);
		if (i != senderIndex && player != NULL &&
			player->GetName() == sender.GetName())
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 DeletePlayer (private)

 ******************************************************************************/

void
War2Wiz::DeletePlayer
	(
	const JIndex index
	)
{
	itsPlayerList->SetElement(index, NULL, JPtrArrayT::kDelete);
}

/******************************************************************************
 WriteCurrentState (private)

	Write all state required by newly connected player.

 ******************************************************************************/

void
War2Wiz::WriteCurrentState
	(
	std::ostream&		data,
	const JIndex	excludeIndex
	)
	const
{
	const JSize playerCount = itsPlayerList->GetElementCount();
	data << playerCount;

	for (JIndex i=1; i<=playerCount; i++)
		{
		const JBoolean exists = JI2B(PlayerIndexValid(i) && i != excludeIndex);
		data << ' ' << exists;
		if (exists)
			{
			data << ' ';
			(itsPlayerList->GetElement(i))->WriteState(data);
			}
		}
}
