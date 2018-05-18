/******************************************************************************
 test_jListUtil.cc

	Program to test jListUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JListUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(AdjustIndexAfterInsert)
{
	JIndex i;

	i = 1;
	JAdjustIndexAfterInsert(1, 3, &i);
	JAssertEqual(4, i);

	i = 1;
	JAdjustIndexAfterInsert(2, 3, &i);
	JAssertEqual(1, i);

	i = 5;
	JAdjustIndexAfterInsert(4, 3, &i);
	JAssertEqual(8, i);

	i = 5;
	JAdjustIndexAfterInsert(5, 3, &i);
	JAssertEqual(8, i);

	i = 5;
	JAdjustIndexAfterInsert(6, 3, &i);
	JAssertEqual(5, i);
}

JTEST(AdjustIndexAfterRemove)
{
	JIndex i;

	i = 1;
	JBoolean ok = JAdjustIndexAfterRemove(1, 3, &i);
	JAssertFalse(ok);

	i = 2;
	ok = JAdjustIndexAfterRemove(1, 3, &i);
	JAssertFalse(ok);

	i = 4;
	ok = JAdjustIndexAfterRemove(1, 3, &i);
	JAssertTrue(ok);
	JAssertEqual(1, i);

	i = 5;
	ok = JAdjustIndexAfterRemove(2, 3, &i);
	JAssertTrue(ok);
	JAssertEqual(2, i);

	i = 5;
	ok = JAdjustIndexAfterRemove(3, 3, &i);
	JAssertFalse(ok);

	i = 5;
	ok = JAdjustIndexAfterRemove(6, 3, &i);
	JAssertTrue(ok);
	JAssertEqual(5, i);
}

JTEST(AdjustIndexAfterMove)
{
	JIndex i;

	i = 1;
	JAdjustIndexAfterMove(1, 5, &i);
	JAssertEqual(5, i);

	i = 3;
	JAdjustIndexAfterMove(1, 5, &i);
	JAssertEqual(2, 2);

	i = 5;
	JAdjustIndexAfterMove(1, 5, &i);
	JAssertEqual(4, i);

	i = 6;
	JAdjustIndexAfterMove(1, 5, &i);
	JAssertEqual(6, i);

	i = 5;
	JAdjustIndexAfterMove(2, 3, &i);
	JAssertEqual(5, i);

	i = 5;
	JAdjustIndexAfterMove(5, 6, &i);
	JAssertEqual(6, i);

	i = 5;
	JAdjustIndexAfterMove(11, 15, &i);
	JAssertEqual(5, i);
}

JTEST(JAdjustIndexAfterSwap)
{
	JIndex i;

	i = 1;
	JAdjustIndexAfterSwap(1, 5, &i);
	JAssertEqual(5, i);

	i = 3;
	JAdjustIndexAfterSwap(1, 5, &i);
	JAssertEqual(3, i);

	i = 5;
	JAdjustIndexAfterSwap(1, 5, &i);
	JAssertEqual(1, i);

	i = 6;
	JAdjustIndexAfterSwap(1, 5, &i);
	JAssertEqual(6, i);
}
