/******************************************************************************
 JBroadcastTester.h

	Interface for JBroadcastTester class.

	Copyright (C) 2016 by John Lindal.

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

	virtual ~JBroadcastTester();

	void	Expect(const JUtf8Byte* type);
	void	ExpectGoingAway();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JQueue<const JUtf8Byte*, JArray<const JUtf8Byte*> >	itsExpectedMessageTypes;
	JBoolean											itsExpectGoingAwayFlag;
};

#endif
