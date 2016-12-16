/******************************************************************************
 WarPlayer.cpp

	Implements server specific functions.

	BASE CLASS = WWPlayer

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "WarPlayer.h"
#include "WarSocket.h"
#include "warGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WarPlayer::WarPlayer
	(
	const JIndex	playerIndex,
	WarSocket*		socket
	)
	:
	WWPlayer(playerIndex)
{
	itsSocket = socket;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WarPlayer::~WarPlayer()
{
	delete itsSocket;
}

/******************************************************************************
 WriteState

 ******************************************************************************/

void
WarPlayer::WriteState
	(
	std::ostream& output
	)
	const
{
	output << itsPlayerIndex;
	output << ' ' << itsName;
	if (!(WarGetSetup())->GameIsAnonymous())
		{
		output << ' ' << itsEmail;
		}
	else
		{
		output << ' ' << JString();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WarPlayer::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	WWPlayer::Receive(sender, message);
}
