#include <iostream>

class A
{
public:

	A() { };
	virtual ~A() { };

private:

	char a;
};

class B : public A
{
public:

	B() { };
	virtual ~B() { };

private:

	long b;
};

int main()
{
	B b;
	cout << &b << endl;
	A* a = &b;
	cout << a << endl;
	B* b1 = dynamic_cast<B*>(a);
	cout << b1 << endl;

	A a2;
	A* a3 = &a2;
	B* b2 = dynamic_cast<B*>(a3);
	cout << b2 << endl;

	return 0;
}
