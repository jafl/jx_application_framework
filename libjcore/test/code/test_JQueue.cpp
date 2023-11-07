/******************************************************************************
 test_JQueue.cpp

	Program to test JQueue class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JQueue.h"
#include "JArray.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Long)
{
	JQueue<long, JArray<long> > a1;
	JAssertEqual(0, a1.GetItemCount());

	for (long i : { 1,2,3,4,5 })
	{
		a1.Append(i);
	}
	JAssertEqual(5, a1.GetItemCount());

	JQueue<long, JArray<long> > a2 = a1;
	JAssertEqual(5, a2.GetItemCount());

	long v1[] = { 1, 2, 3, 4, 5 };
	long i    = 0;
	while (!a1.IsEmpty())
	{
		JAssertEqual(v1[i], a1.GetNext());
		i++;
	}

	JAssertEqual(0, a1.GetItemCount());
	JAssertEqual(5, a2.GetItemCount());

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

	JAssertEqual(3, a2.GetItemCount());
	JAssertEqual(3, a2.PeekNext());
	JAssertEqual(3, a2.GetItemCount());

	a2.Discard(2);
	JAssertEqual(1, a2.GetItemCount());

	a2.Flush();
	JAssertEqual(0, a2.GetItemCount());
}
