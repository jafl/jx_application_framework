/******************************************************************************
 test_jXEventUtil.cpp

	Test X event utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jx/jXEventUtil.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Ctrl)
{
	JAssertEqual(3, (long) JXCtrl('C'));
	JAssertEqual(19, (long) JXCtrl('S'));
}
