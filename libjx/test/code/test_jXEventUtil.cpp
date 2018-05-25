/******************************************************************************
 test_jXEventUtil.cc

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

JTEST(IsPrint)
{
	JAssertFalse(JXIsPrint(27));	// esc
	JAssertTrue(JXIsPrint(97));		// a
	JAssertFalse(JXIsPrint(210));	// out of range
}

JTEST(Ctrl)
{
	JAssertEqual(3, (long) JXCtrl('C'));
	JAssertEqual(19, (long) JXCtrl('S'));
}
