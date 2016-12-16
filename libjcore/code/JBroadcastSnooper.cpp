/******************************************************************************
 JBroadcastSnooper.cpp

	Test class that prints out each message received from objects
	that it listens to.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JBroadcastSnooper.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBroadcastSnooper::JBroadcastSnooper
	(
	const JBroadcaster* obj
	)
{
	ListenTo(obj);
}

/******************************************************************************
 Receive (virtual protected)

	Display the message.

 ******************************************************************************/

void
JBroadcastSnooper::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	std::cout << message.GetType() << " broadcast from ";
	std::cout.setf(std::ios::hex, std::ios::basefield);
	std::cout << static_cast<void*>(sender) << std::endl;
	std::cout.setf(std::ios::dec, std::ios::basefield);
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JBroadcastSnooper::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	std::cout.setf(std::ios::hex, std::ios::basefield);
	std::cout << static_cast<void*>(sender);
	std::cout.setf(std::ios::dec, std::ios::basefield);
	std::cout << " deleted" << std::endl;
}
