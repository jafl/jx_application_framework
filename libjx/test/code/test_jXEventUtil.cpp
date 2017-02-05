/******************************************************************************
 test_jXEventUtil.cc

	Test X event utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <jXUtil.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(IsPrint)
{
	JAssertFalse(JXIsPrint(27));	// esc
	JAssertTrue(JXIsPrint(97));		// a
	JAssertFalse(JXIsPrint(210));	// out of range
}

JTEST(Ctrl)
{
	JAssertEqual(3, JXCtrl('C'));
	JAssertEqual(115, JXCtrl('s'));
}
