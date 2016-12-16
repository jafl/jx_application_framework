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
	std::std::cout << &b << std::std::endl;
	A* a = &b;
	std::std::cout << a << std::std::endl;
	B* b1 = dynamic_cast<B*>(a);
	std::std::cout << b1 << std::std::endl;

	A a2;
	A* a3 = &a2;
	B* b2 = dynamic_cast<B*>(a3);
	std::std::cout << b2 << std::std::endl;

	return 0;
}
