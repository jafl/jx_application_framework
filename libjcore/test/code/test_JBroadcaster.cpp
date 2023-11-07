/******************************************************************************
 test_JBroadcaster.cpp

	Program to test JBroadcaster class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JBroadcastTester.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

class Test : virtual public JBroadcaster
{
public:

	template <class T>
	void
	Bcast(const T& msg)
	{
		Broadcast(msg);
	};
};

class TestFunction : virtual public JBroadcaster
{
public:

	template <class T>
	void	Sub(const JBroadcaster* sender,
				const std::function<void(const T&)>& f)
	{
		ListenTo(sender, f);
	}
};

JTEST(Broadcaster)
{
	Test t3;

	JBroadcastTester t1(&t3);
	JBroadcastTester t2(&t3);

	t1.Expect(JListT::kSorted);
	t1.Expect(JListT::kItemMoved);

	t2.Expect(JListT::kSorted);
	t2.Expect(JListT::kItemMoved);

	JListT::Sorted msg1;
	t3.Bcast(msg1);

	JListT::ItemMoved msg2(3, 5);
	t3.Bcast(msg2);
}

JTEST(BroadcasterFunctions)
{
	Test t3;

	bool t1_1 = false,
		 t2_1 = false,
		 t2_2 = false;

	TestFunction t1;
	t1.Sub(&t3, std::function([&t1_1](const JListT::Sorted& msg)
	{
		t1_1 = true;
	}));

	TestFunction t2;
	t2.Sub(&t3, std::function([&t2_1](const JListT::Sorted& msg)
	{
		t2_1 = true;
	}));
	t2.Sub(&t3, std::function([&t2_2](const JListT::ItemMoved& msg)
	{
		t2_2 = true;
	}));

	JListT::Sorted msg1;
	t3.Bcast(msg1);

	JListT::ItemMoved msg2(3, 5);
	t3.Bcast(msg2);

	JAssertTrue(t1_1);
	JAssertTrue(t2_1);
	JAssertTrue(t2_2);
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
