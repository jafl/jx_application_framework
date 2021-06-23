/******************************************************************************
 XDSocket.cpp

	Socket that connects to Xdebug.

	BASE CLASS = JMessageProtocol

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDSocket.h"
#include "XDLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

const time_t kClientDeadTime = 5;	// seconds

/******************************************************************************
 Constructor

 ******************************************************************************/

XDSocket::XDSocket()
	:
	itsTimerID(-1)
{
	SetProtocol("\0", 1, "\1", 1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDSocket::~XDSocket()
{
	SendDisconnect();
}

/******************************************************************************
 open (virtual)

	This is called when Xdebug connects from a browser.

 ******************************************************************************/

int
XDSocket::open
	(
	void* data
	)
{
	const int result = JMessageProtocol<ACE_SOCK_STREAM>::open(data);
	if (result == 0)
		{
		dynamic_cast<XDLink*>(CMGetLink())->ConnectionEstablished(this);
		}
	return result;
}

/******************************************************************************
 StartTimer

 ******************************************************************************/

void
XDSocket::StartTimer()
{
	StopTimer();

//	itsTimerID = (reactor())->schedule_timer(this, nullptr, ACE_Time_Value(kClientDeadTime));
//	if (itsTimerID == -1)
//		{
//		std::cerr << "XDSocket::StartTimer() is unable to schedule timeout" << std::endl;
//		}
}

/******************************************************************************
 StopTimer

 ******************************************************************************/

void
XDSocket::StopTimer()
{
	reactor()->cancel_timer(itsTimerID);
}

/******************************************************************************
 handle_timeout (virtual)

	Notify XDLink that session seems to be closed.

 ******************************************************************************/

int
XDSocket::handle_timeout
	(
	const ACE_Time_Value&	time,
	const void*				data
	)
{
	close();
	return 0;
}

/******************************************************************************
 handle_close (virtual)

	This is called when the connection is closed.

 ******************************************************************************/

int
XDSocket::handle_close
	(
	ACE_HANDLE			h,
	ACE_Reactor_Mask	m
	)
{
	dynamic_cast<XDLink*>(CMGetLink())->ConnectionFinished(this);
	return JMessageProtocol<ACE_SOCK_STREAM>::handle_close(h, m);
}
