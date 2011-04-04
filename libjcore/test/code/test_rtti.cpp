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
	std::cout << &b << std::endl;
	A* a = &b;
	std::cout << a << std::endl;
	B* b1 = dynamic_cast<B*>(a);
	std::cout << b1 << std::endl;

	A a2;
	A* a3 = &a2;
	B* b2 = dynamic_cast<B*>(a3);
	std::cout << b2 << std::endl;

	return 0;
}
