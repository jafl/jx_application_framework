/******************************************************************************
 test_jVCSUtil.cpp

	Program to test version control utils.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <jVCSUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Directory)
{
	JAssertTrue(JIsVCSDirectory(JString(".git", JString::kNoCopy)));
	JAssertEqual(kJGitType, JGetVCSType(JGetCurrentDirectory()));
	JAssertTrue(JIsManagedByVCS(JString("./Make.files", JString::kNoCopy)));
}
