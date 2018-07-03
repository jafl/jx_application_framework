/******************************************************************************
 test_jXEventUtil.cpp

	Test X event utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jXEventUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Ctrl)
{
	JAssertEqual(3, (long) JXCtrl('C'));
	JAssertEqual(19, (long) JXCtrl('S'));
}
