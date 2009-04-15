/******************************************************************************
 test_JBroadcaster.cc

	Program to test JBroadcaster class.

	Written by John Lindal.

 ******************************************************************************/

#include <JBroadcaster.h>
#include <JOrderedSet.h>
#include <jCommandLine.h>
#include <jAssert.h>

class Test : virtual public JBroadcaster
{
public:

	Test() { };

	void
	Listen(Test* t)
	{
		ListenTo(t);
	};

	void
	Bcast(Message& msg)
	{
		Broadcast(msg);
	};

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
};

Test t1, t2, t3;

void
Test::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	cout << this << " received message of type " << message.GetType() << endl;

	StopListening(&t3);
	ListenTo(&t3);
}

int
main()
{
	t1.Listen(&t3);
	t2.Listen(&t3);
	JOrderedSetT::Sorted msg;
	t3.Bcast(msg);
	return 0;
}
