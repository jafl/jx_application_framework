/******************************************************************************
 JBroadcastSnooper.h

	Interface for JBroadcastSnooper class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBroadcastSnooper
#define _H_JBroadcastSnooper

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
