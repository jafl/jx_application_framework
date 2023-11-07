/******************************************************************************
 JBroadcastTester.cpp

	Test class that verifies the expected messages.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "JBroadcastTester.h"
#include "JTestManager.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JBroadcastTester::JBroadcastTester
	(
	const JBroadcaster* obj
	)
	:
	itsExpectGoingAwayFlag(false)
{
	ListenTo(obj);
}

/******************************************************************************
 Destructor

	We don't bother to delete the validator objects, because this is only
	for functional testing.  Everything is cleaned up when the program
	exits.

 ******************************************************************************/

JBroadcastTester::~JBroadcastTester()
{
	JAssertEqualWithMessage(0UL, itsExpectedMessages.GetItemCount(),
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
	v.validator = jnew std::function(validator);
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
	const bool empty = itsExpectedMessages.IsEmpty();
	JAssertFalseWithMessage(empty, message.GetType());

	if (!empty)
	{
		const Validation v = itsExpectedMessages.GetNext();
		JAssertStringsEqual(v.type, message.GetType());

		if (v.validator != nullptr)
		{
			(*v.validator)(message);
		}
	}
}

/******************************************************************************
 ExpectGoingAway

 ******************************************************************************/

void
JBroadcastTester::ExpectGoingAway()
{
	itsExpectGoingAwayFlag = true;
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

/******************************************************************************
 NOP (static private)

 ******************************************************************************/

void
JBroadcastTester::NOP
	(
	const Message& m
	)
{
}
