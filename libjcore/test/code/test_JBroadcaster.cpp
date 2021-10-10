/******************************************************************************
 test_JBroadcaster.cpp

	Program to test JBroadcaster class.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JBroadcastTester.h>
#include <jx-af/jcore/jAssert.h>

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
	~B() override { };
	JUtf8Byte foo() override { return 'B'; };
};

class C : virtual public A
{
public:
	~C() override { };
	JUtf8Byte foo() override { return 'C'; };
};

class D : public B, public C
{
public:
	~D() override { };
	JUtf8Byte foo() override { return 'D'; };
};

JTEST(DiamondVirtualInheritance)
{
	A* obj = jnew D();
	JAssertEqual('D', obj->foo());
	B* b = dynamic_cast<B*>(obj);
	JAssertEqual('D', b->foo());
	D* d = dynamic_cast<D*>(obj);
	JAssertEqual('D', d->foo());
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
	*((PointerSize**) arg) = nullptr;
}

JTEST(voidstar)
{
	Test t1;
	Test* t4 = &t1;
	JAssertNotNull(t4);
	clearPointer((void*) &t4);
	JAssertNull(t4);
}
