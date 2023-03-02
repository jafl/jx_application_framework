/******************************************************************************
 JBroadcastSnooper.h

	Interface for JBroadcastSnooper class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBroadcastSnooper
#define _H_JBroadcastSnooper

#include "JBroadcaster.h"

class JBroadcastSnooper : virtual public JBroadcaster
{
public:

	JBroadcastSnooper(const JBroadcaster* obj);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;
};

#endif
