class A : private D
{
public:
void twee();
protected:
void foobar();
void foo();
void bar();

	class A2
	{
	public:
	void foo();
	};
}

template <class T>
class A1
{
public:
void foo();
}

void Z:: foo() {};

void A ::  foobar() {};

void A
::
foo() {};

template <class T> void A1<T>::foo() {};

void A
::
A2
::
foo() {};
