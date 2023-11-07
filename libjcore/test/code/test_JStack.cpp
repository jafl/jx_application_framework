/******************************************************************************
 test_JStack.cpp

	Program to test JStack class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JStack.h"
#include "JArray.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Long)
{
	JStack<long, JArray<long> > a1;
	JAssertEqual(0, a1.GetItemCount());

	for (long i : { 1,2,3,4,5 })
	{
		a1.Push(i);
	}
	JAssertEqual(5, a1.GetItemCount());

	JStack<long, JArray<long> > a2 = a1;
	JAssertEqual(5, a2.GetItemCount());

	long v1[] = { 5, 4, 3, 2, 1 };
	long i    = 0;
	while (!a1.IsEmpty())
	{
		JAssertEqual(v1[i], a1.Pop());
		i++;
	}

	JAssertEqual(0, a1.GetItemCount());
	JAssertEqual(5, a2.GetItemCount());

	long v2[] = { 5, 4 };
	i         = 0;
	long j;
	do
	{
		j = a2.Pop();
		JAssertEqual(v2[i], j);
		i++;
	}
		while (j > 4);

	JAssertEqual(3, a2.GetItemCount());

	a2.Unwind(2);
	JAssertEqual(1, a2.GetItemCount());

	a2.Clear();
	JAssertEqual(0, a2.GetItemCount());
}
