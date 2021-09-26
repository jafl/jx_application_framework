/******************************************************************************
 test_jVCSUtil.cpp

	Program to test version control utils.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

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
