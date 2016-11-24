/******************************************************************************
 JBroadcastTester.h

	Interface for JBroadcastTester class.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBroadcastTester
#define _H_JBroadcastTester

#include <JBroadcaster.h>
#include <JQueue.h>
#include <JArray.h>

class JBroadcastTester : virtual public JBroadcaster
{
public:

	JBroadcastTester(const JBroadcaster* obj);

	void	Expect(const JUtf8Byte* type);
	void	ExpectGoingAway();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JQueue<const JUtf8Byte*, JArray<const JUtf8Byte*> >	itsExpectedMessageTypes;
	JBoolean											itsExpectGoingAwayFlag;
};

#endif
