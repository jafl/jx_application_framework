/******************************************************************************
 Wiz2War.cpp

	Maintains a connection to the War2Wiz object in the server.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "Wiz2War.h"
#include "WizPlayer.h"
#include "wwUtil.h"
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <JThisProcess.h>
#include <JLatentPG.h>
#include <jSysUtil.h>
#include <jErrno.h>
#include <stdlib.h>
#include <jAssert.h>

const Time kServerPingInterval = 60 * 1000;		// 1 minute (milliseconds)

// JBroadcaster message types

const JCharacter* Wiz2War::kConnectionAccepted = "ConnectionAccepted::Wiz2War";
const JCharacter* Wiz2War::kConnectionRefused  = "ConnectionRefused::Wiz2War";
const JCharacter* Wiz2War::kDisconnect         = "Disconnect::Wiz2War";

const JCharacter* Wiz2War::kPlayerJoined = "PlayerJoined::Wiz2War";
const JCharacter* Wiz2War::kPlayerLeft   = "PlayerLeft::Wiz2War";

const JCharacter* Wiz2War::kChatMessage = "ChatMessage::Wiz2War";

/******************************************************************************
 Constructor

 ******************************************************************************/

Wiz2War::Wiz2War()
{
	itsSocket      = NULL;
	itsPlayerIndex = 0;
	itsPlayerList  = NULL;

	itsPingTask = new JXTimerTask(kServerPingInterval, kJFalse);
	assert( itsPingTask != NULL );
	(JXGetApplication())->InstallIdleTask(itsPingTask);
	ListenTo(itsPingTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Wiz2War::~Wiz2War()
{
	if (itsSocket != NULL)
		{
		itsSocket->SendDisconnect();
		DeleteSocket();
		}

	delete itsPlayerList;
	delete itsPingTask;
}

/******************************************************************************
 GetPlayer

 ******************************************************************************/

JBoolean
Wiz2War::GetPlayer
	(
	const JIndex	index,
	WizPlayer**		player
	)
	const
{
	if (itsPlayerList->IndexValid(index))
		{
		*player = itsPlayerList->NthElement(index);
		return JI2B( *player != NULL );
		}
	else
		{
		*player = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 SendChatMessage

 ******************************************************************************/

void
Wiz2War::SendChatMessage
	(
	const JIndex		receiverIndex,
	const JCharacter*	text
	)
{
	ostrstream data;
	data << receiverIndex;
	data << ' ' << JString(text);
	SendMessage(kWWChatMessage, data);
}

/******************************************************************************
 SendMessage (private)

 ******************************************************************************/

void
Wiz2War::SendMessage
	(
	const JIndex	id,
	ostrstream&		data
	)
{
	data << ends;
	SendMessage(id, data.str());
	JUnfreeze(data);
}

void
Wiz2War::SendMessage
	(
	const JIndex		id,
	const JCharacter*	data
	)
{
	assert( itsSocket != NULL );

	JString msg(id, 0);
	msg += " ";
	msg += data;
	itsSocket->SendMessage(msg);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Wiz2War::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSocket && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveMessage();
		}

	else if (sender == itsSocket && message.Is(JMessageProtocolT::kReceivedDisconnect))
		{
		Broadcast(Disconnect());
		}

	else if (sender == itsPingTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		if (itsSocket != NULL)
			{
			SendMessage(kWWClientAlive, "");
			}
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveMessage (private)

	Decode and dispatch message from the server.

 ******************************************************************************/

void
Wiz2War::ReceiveMessage()
{
	JString s;
	const JBoolean ok = itsSocket->GetNextMessage(&s);
	assert( ok );

	jistrstream(input, s.GetCString(), s.GetLength());
	WWMessageType type;
	input >> type;

	if (type == kWWConnectionEstablished)
		{
		JBoolean accept;
		input >> accept;
		if (accept)
			{
			JSize maxPlayerCount;
			input >> maxPlayerCount >> itsPlayerIndex;
			Broadcast(ConnectionAccepted(maxPlayerCount, itsPlayerIndex));
			ReadCurrentState(input);
			}
		else
			{
			WWConnectionErrorType error;
			input >> error;
			Broadcast(ConnectionRefused(error));
			}
		}

	else if (type == kWWPlayerJoined)
		{
		JIndex playerIndex;
		input >> playerIndex;

		WizPlayer* player = new WizPlayer(input);
		assert( player != NULL );
		if (itsPlayerList->IndexValid(playerIndex))
			{
			itsPlayerList->SetElement(playerIndex, player, JPtrArrayT::kDelete);
			}
		else
			{
			assert( playerIndex == itsPlayerList->GetElementCount() + 1 );
			itsPlayerList->Append(player);
			}

		Broadcast(PlayerJoined(playerIndex, *player, kJTrue));
		}

	else if (type == kWWPlayerLeft)
		{
		JIndex playerIndex;
		JBoolean timeout;
		input >> playerIndex >> timeout;
		Broadcast(PlayerLeft(playerIndex, timeout));

		itsPlayerList->SetElement(playerIndex, NULL, JPtrArrayT::kDelete);
		}

	else if (type == kWWChatMessage)
		{
		JIndex senderIndex;
		JString msg;
		input >> senderIndex >> msg;
		Broadcast(ChatMessage(senderIndex, msg));
		}
}

/******************************************************************************
 ReadCurrentState (private)

	Write all state required by newly connected player.

 ******************************************************************************/

void
Wiz2War::ReadCurrentState
	(
	istream& input
	)
{
JIndex i;

	assert( itsPlayerList == NULL );

	itsPlayerList = new JPtrArray<WizPlayer>(JPtrArrayT::kDeleteAll);
	assert( itsPlayerList != NULL );

	JSize playerCount;
	input >> playerCount;

	for (i=1; i<=playerCount; i++)
		{
		WizPlayer* player = NULL;

		JBoolean exists;
		input >> exists;
		if (exists)
			{
			player = new WizPlayer(input);
			assert( player != NULL );
			}

		itsPlayerList->Append(player);
		}

	for (i=1; i<=playerCount; i++)
		{
		const WizPlayer* player = itsPlayerList->NthElement(i);
		if (player != NULL)
			{
			Broadcast(PlayerJoined(i, *player, kJFalse));
			}
		}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
Wiz2War::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (itsSocket != NULL)
		{
		itsSocket->SendDisconnect();
		}
}

/******************************************************************************
 Connect

	Attempt to connect to the server.  This is not done in the constructor
	in order to allow a dialog asking the user for the server address.

 ******************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

void
Wiz2War::Connect
	(
	const JCharacter* serverAddr,
	const JCharacter* name
	)
{
	DeleteSocket();		// in case of retry
	itsSocket = new WWSocket;
	assert( itsSocket != NULL );

	JLatentPG pg;
	pg.VariableLengthProcessBeginning("Connecting to WizWar server...", kJTrue, kJFalse);

	ACE_Connector<WWSocket, ACE_SOCK_CONNECTOR> connector;
	const ACE_INET_Addr addr(serverAddr);
	const ACE_Synch_Options options(ACE_Synch_Options::USE_TIMEOUT, 1);
	while (1)
		{
		const int result = connector.connect(itsSocket, addr, options);
		if (result == 0)
			{
			break;
			}
		else if (result == -1)
			{
			JString msg = "Unable to connect to the server.\n\nerror #";
			msg += JString(jerrno(), JString::kBase10);
			(JGetUserNotification())->ReportError(msg);
			JThisProcess::Exit(1);
			}

		if (!pg.IncrementProgress())
			{
			JThisProcess::Exit(1);
			}
		}

	pg.ProcessFinished();

	WWSetProtocol(itsSocket);
	ListenTo(itsSocket);

	const JString email = JGetUserName() + "@" + JGetHostName();

	ostrstream data;
	data << kWWCurrentProtocolVersion;
	data << ' ' << JString(name);
	data << ' ' << email;
	SendMessage(kWWClientIdentity, data);

	// wait for kWWConnectionEstablished
}

/******************************************************************************
 DeleteSocket (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
Wiz2War::DeleteSocket()
{
	delete itsSocket;
	itsSocket = NULL;
}
