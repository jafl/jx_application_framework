/******************************************************************************
 JBroadcastTester.cpp

	Test class that verifies the expected messages.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "JBroadcastTester.h"
#include "JTestManager.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JBroadcastTester::JBroadcastTester
	(
	const JBroadcaster* obj
	)
	:
	itsExpectGoingAwayFlag(kJFalse)
{
	ListenTo(obj);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBroadcastTester::~JBroadcastTester()
{
	JAssertEqualWithMessage(0, itsExpectedMessageTypes.GetElementCount(),
							"expected more JBroadcaster messages");
}

/******************************************************************************
 Expect

 ******************************************************************************/

void
JBroadcastTester::Expect
	(
	const JUtf8Byte* type
	)
{
	itsExpectedMessageTypes.Append(type);
}

/******************************************************************************
 Receive (virtual protected)

	Display the message.

 ******************************************************************************/

void
JBroadcastTester::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	const JBoolean empty = itsExpectedMessageTypes.IsEmpty();
	JAssertFalseWithMessage(empty, message.GetType());

	if (!empty)
		{
		JAssertStringsEqual(itsExpectedMessageTypes.GetNext(), message.GetType());
		}
}

/******************************************************************************
 ExpectGoingAway

 ******************************************************************************/

void
JBroadcastTester::ExpectGoingAway()
{
	itsExpectGoingAwayFlag = kJTrue;
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JBroadcastTester::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	JAssertTrue(itsExpectGoingAwayFlag);
	JAssertTrue(itsExpectedMessageTypes.IsEmpty());
}
