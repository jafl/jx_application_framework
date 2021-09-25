/******************************************************************************
 test_JMemoryManager.cpp

	Program to test JMemoryManager multi-threading - required by liblldb :(

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JKLRand.h>
#include <thread>
#include <jAssert.h>

const JSize kThreadCount = 100;
const JSize kDataCount   = 1000;
const JSize kIterCount   = 1000;

int main()
{
	return JTestManager::Execute();
}

void test()
{
	char* data[kDataCount];

	for (JUnsignedOffset i=0; i<kDataCount; i++)
	{
		data[i] = jnew char[10];
	}

	JKLRand r;
	for (JIndex i=1; i<kIterCount; i++)
	{
		const JUnsignedOffset j = r.UniformLong(1, kDataCount) - 1;
		jdelete [] data[j];
		data[j] = jnew char[10];
	}
}

JTEST(Threads)
{
	std::thread* t[kThreadCount];

	for (JUnsignedOffset i=0; i<kThreadCount; i++)
	{
		t[i] = jnew std::thread(test);
	}

	for (JUnsignedOffset i=0; i<kThreadCount; i++)
	{
		t[i]->join();
	}
}
