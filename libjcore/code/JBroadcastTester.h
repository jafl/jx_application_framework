/******************************************************************************
 JBroadcastTester.h

	Interface for JBroadcastTester class.

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBroadcastTester
#define _H_JBroadcastTester

#include "JBroadcaster.h"
#include "JQueue.h"
#include "JArray.h"
#include <functional>

class JBroadcastTester : virtual public JBroadcaster
{
public:

	JBroadcastTester(const JBroadcaster* obj);

	~JBroadcastTester() override;

	void	Expect(const JUtf8Byte* type, std::function<void(const Message&)> validator = NOP);
	void	ExpectGoingAway();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

public:

	struct Validation
	{
		const JUtf8Byte*						type;
		std::function<void(const Message&)>*	validator;
	};

private:

	JQueue<Validation, JArray<Validation> >	itsExpectedMessages;
	bool									itsExpectGoingAwayFlag;

private:

	static void	NOP(const Message&);
};

#endif
