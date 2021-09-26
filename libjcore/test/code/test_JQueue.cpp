/******************************************************************************
 test_JQueue.cpp

	Program to test JQueue class.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JQueue.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Long)
{
	JQueue<long, JArray<long> > a1;
	JAssertEqual(0, a1.GetElementCount());

	for (long i : { 1,2,3,4,5 })
	{
		a1.Append(i);
	}
	JAssertEqual(5, a1.GetElementCount());

	JQueue<long, JArray<long> > a2 = a1;
	JAssertEqual(5, a2.GetElementCount());

	long v1[] = { 1, 2, 3, 4, 5 };
	long i    = 0;
	while (!a1.IsEmpty())
	{
		JAssertEqual(v1[i], a1.GetNext());
		i++;
	}

	JAssertEqual(0, a1.GetElementCount());
	JAssertEqual(5, a2.GetElementCount());

	long v2[] = { 1, 2 };
	i         = 0;
	long j;
	do
	{
		j = a2.GetNext();
		JAssertEqual(v2[i], j);
		i++;
	}
		while (j < 2);

	JAssertEqual(3, a2.GetElementCount());
	JAssertEqual(3, a2.PeekNext());
	JAssertEqual(3, a2.GetElementCount());

	a2.Discard(2);
	JAssertEqual(1, a2.GetElementCount());

	a2.Flush();
	JAssertEqual(0, a2.GetElementCount());
}
