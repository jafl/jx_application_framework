/******************************************************************************
 JBroadcastSnooper.cpp

	Test class that prints out each message received from objects
	that it listens to.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
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
	cout << message.GetType() << " broadcast from ";
	cout.setf(ios::hex, ios::basefield);
	cout << static_cast<void*>(sender) << endl;
	cout.setf(ios::dec, ios::basefield);
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
	cout.setf(ios::hex, ios::basefield);
	cout << static_cast<void*>(sender);
	cout.setf(ios::dec, ios::basefield);
	cout << " deleted" << endl;
}
