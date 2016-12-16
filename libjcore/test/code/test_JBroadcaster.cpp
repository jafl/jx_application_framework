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
	std::cout << this << " received message of type " << message.GetType() << std::endl;

	StopListening(&t3);
	ListenTo(&t3);
}

class A
{
public:
	virtual ~A() { };
	virtual void foo() { std::cout << 'A' << std::endl; };
};

class B : virtual public A
{
public:
	virtual ~B() { };
	virtual void foo() { std::cout << 'B' << std::endl; };
};

class C : virtual public A
{
public:
	virtual ~C() { };
	virtual void foo() { std::cout << 'C' << std::endl; };
};

class D : public B, public C
{
public:
	virtual ~D() { };
	virtual void foo() { std::cout << 'D' << std::endl; };
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
	std::cout << t4 << std::endl;
	clearPointer((void*) &t4);
	std::cout << t4 << std::endl;

	return 0;
}
