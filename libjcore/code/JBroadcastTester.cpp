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
	JAssertEqualWithMessage(0, itsExpectedMessages.GetElementCount(),
							"expected more JBroadcaster messages");
}

/******************************************************************************
 Expect

 ******************************************************************************/

void
JBroadcastTester::Expect
	(
	const JUtf8Byte*					type,
	std::function<void(const Message&)>	validator
	)
{
	Validation v;
	v.type      = type;
	v.validator = validator;
	itsExpectedMessages.Append(v);
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
	const JBoolean empty = itsExpectedMessages.IsEmpty();
	JAssertFalseWithMessage(empty, message.GetType());

	if (!empty)
		{
		const Validation v = itsExpectedMessages.GetNext();
		JAssertStringsEqual(v.type, message.GetType());

		if (v.validator != nullptr)
			{
			v.validator(message);
			}
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
	JAssertTrue(itsExpectedMessages.IsEmpty());
}
