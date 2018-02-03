/******************************************************************************
 WarSocket.cpp

	Socket that connects to a client.

	BASE CLASS = WWSocket

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "WarSocket.h"
#include "warGlobals.h"
#include <jTime.h>
#include <jAssert.h>

const time_t kClientDeadTime = 5*60;	// 5 minutes (seconds)

// JBroadcaster message types

const JCharacter* WarSocket::kTimeout = "Timeout::WarSocket";

/******************************************************************************
 Constructor

 ******************************************************************************/

WarSocket::WarSocket()
{
	itsTimerID = -1;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WarSocket::~WarSocket()
{
	SendDisconnect();
}

/******************************************************************************
 open (virtual)

	This is called when a Wiz2War object connects from a client.

 ******************************************************************************/

int
WarSocket::open
	(
	void* data
	)
{
	const int result = WWSocket::open(data);
	if (result == 0)
		{
		(WarGetConnectionManager())->ConnectionEstablished(this);
		ResetTimer();
		}
	return result;
}

/******************************************************************************
 handle_timeout (virtual)

	Notify Wiz2War that the client seems to be dead.

 ******************************************************************************/

int
WarSocket::handle_timeout
	(
	const ACE_Time_Value&	time,
	const void*				data
	)
{
	Broadcast(Timeout());	// can delete us
	return 0;
}

/******************************************************************************
 ResetTimer

 ******************************************************************************/

void
WarSocket::ResetTimer()
{
	(reactor())->cancel_timer(itsTimerID);
	itsTimerID = (reactor())->schedule_timer(this, NULL, ACE_Time_Value(kClientDeadTime));
	if (itsTimerID == -1)
		{
		std::cerr << "WarSocket::ResetTimer() is unable to schedule timeout" << std::endl;
		}
}
