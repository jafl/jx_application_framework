/******************************************************************************
 WarSocket.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WarSocket
#define _H_WarSocket

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "WWSocket.h"

class WarSocket : public WWSocket
{
public:

	WarSocket();

	virtual	~WarSocket();

	virtual int	open(void*);

	virtual int	handle_timeout(const ACE_Time_Value& time, const void*);
	void		ResetTimer();

private:

	long	itsTimerID;

public:

	// Broadcaster messages

	static const JCharacter* kTimeout;

	class Timeout : public JBroadcaster::Message
		{
		public:

			Timeout()
				:
				JBroadcaster::Message(kTimeout)
				{ };
		};
};

#endif
