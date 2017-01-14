/******************************************************************************
 test_jVCSUtil.cc

	Program to test version control utils.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <jVCSUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(Directory)
{
	JAssertTrue(JIsVCSDirectory(JString(".git", 0, kJFalse)));
	JAssertEqual(kJGitType, JGetVCSType(JGetCurrentDirectory()));
	JAssertTrue(JIsManagedByVCS(JString("./Make.files", 0, kJFalse)));
}
