/******************************************************************************
 test_JBroadcaster.cc

	Program to test JBroadcaster class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JBroadcastTester.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class Test : virtual public JBroadcaster
{
public:

	void
	Bcast(Message& msg)
	{
		Broadcast(msg);
	};
};

JTEST(Broadcaster)
{
	Test t3;

	JBroadcastTester t1(&t3);
	JBroadcastTester t2(&t3);

	t1.Expect(JListT::kSorted);
	t1.Expect(JListT::kElementMoved);

	t2.Expect(JListT::kSorted);
	t2.Expect(JListT::kElementMoved);

	JListT::Sorted msg1;
	t3.Bcast(msg1);

	JListT::ElementMoved msg2(3, 5);
	t3.Bcast(msg2);
}

class A
{
public:
	virtual ~A() { };
	virtual JUtf8Byte foo() { return 'A'; };
};

class B : virtual public A
{
public:
	virtual ~B() { };
	virtual JUtf8Byte foo() { return 'B'; };
};

class C : virtual public A
{
public:
	virtual ~C() { };
	virtual JUtf8Byte foo() { return 'C'; };
};

class D : public B, public C
{
public:
	virtual ~D() { };
	virtual JUtf8Byte foo() { return 'D'; };
};

JTEST(DiamondVirtualInheritance)
{
	A* obj = jnew D();
	JAssertEqual('D', obj->foo());
	B* b = dynamic_cast<B*>(obj);
	JAssertEqual('D', obj->foo());
	D* d = dynamic_cast<D*>(obj);
	JAssertEqual('D', obj->foo());
}

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

JTEST(voidstar)
{
	Test t1;
	Test* t4 = &t1;
	JAssertNotNull(t4);
	clearPointer((void*) &t4);
	JAssertNull(t4);
}
