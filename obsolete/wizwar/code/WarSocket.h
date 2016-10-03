/******************************************************************************
 WarSocket.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WarSocket
#define _H_WarSocket

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
