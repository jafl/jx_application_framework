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

class A
{
public:
	virtual ~A() { };
	virtual void foo() { cout << 'A' << endl; };
};

class B : virtual public A
{
public:
	virtual ~B() { };
	virtual void foo() { cout << 'B' << endl; };
};

class C : virtual public A
{
public:
	virtual ~C() { };
	virtual void foo() { cout << 'C' << endl; };
};

class D : public B, public C
{
public:
	virtual ~D() { };
	virtual void foo() { cout << 'D' << endl; };
};

struct PointerSize
{
	int pointerData;
};

void
clearPointer
	(
	void* arg
	)
{
	*((PointerSize**) arg) = NULL;
}

int
main()
{
	A* obj = jnew D();
	obj->foo();
	B* b = dynamic_cast<B*>(obj);
	b->foo();
	D* d = dynamic_cast<D*>(obj);
	d->foo();

	t1.Listen(&t3);
	t2.Listen(&t3);
	JOrderedSetT::Sorted msg;
	t3.Bcast(msg);

	Test* t4 = &t1;
	cout << t4 << endl;
	clearPointer((void*) &t4);
	cout << t4 << endl;

	return 0;
}
