/******************************************************************************
 testhash.cpp

	Test std library mutexes.

 *****************************************************************************/

#include <thread>
#include <mutex>
#include <iostream>

const int kThreadCount = 10000;

static std::mutex m;
static std::recursive_mutex rm;

class Foo
{
public:

	Foo()
	{
		std::lock_guard mg(m);
		std::cout << "static init" << std::endl;
	}
};

static Foo foo;

void sub()
{
	std::lock_guard rmg(rm);
	std::lock_guard mg(m);
}

void run()
{
	std::lock_guard rmg(rm);
	sub();
}

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "\nTest mutexes" << std::endl;
	std::cout << "system has " << std::thread::hardware_concurrency() << " cores" << std::endl;

	std::atexit(run);

	std::thread* t[kThreadCount];

	for (int i=0; i<kThreadCount; i++)
		{
		t[i] = new std::thread(run);
		}

	for (int i=0; i<kThreadCount; i++)
		{
		t[i]->join();
		delete t[i];
		}

	return 0;
}
