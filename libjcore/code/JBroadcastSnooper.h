/******************************************************************************
 JBroadcastSnooper.h

	Interface for JBroadcastSnooper class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBroadcastSnooper
#define _H_JBroadcastSnooper

#include <JBroadcaster.h>

class JBroadcastSnooper : virtual public JBroadcaster
{
public:

	JBroadcastSnooper(const JBroadcaster* obj);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);
};

#endif
