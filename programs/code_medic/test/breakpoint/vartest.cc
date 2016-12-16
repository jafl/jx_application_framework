#include <iostream.h>

// This program tests for a variable that changes from a struct to a
// pointer and back again to see if the local variables tree parses it
// correctly.

void foo(int a, int b)
{
int c = 2;
c++;
}

void foo1(int a, int b);
void foo2(int a, int b);

int main()
{
int X;

foo1(1, 2);
foo2(3, 4);

float a = 3.2;
	{
	char* a = "this is a string aaaaaaaaaaaaaaaaab";
	std::cout << a << std::endl;
		{
		int  b = 2;
		int* a = &b;
			{
			int  b = 3;
			int& a = b;

			foo(a, b);

			X++;
			}

		X++;
		}

	X++;
	}

X++;
return 0;
}
