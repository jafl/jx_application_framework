/******************************************************************************
 test_JPrefsManager.cpp

	Program to test JPrefsManager class.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "TestPrefsManager.h"
#include "TestPrefObject.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kSignature = "test-jprefsmanager";

int main()
{
	return JTestManager::Execute();
}

JTEST(NewFile)
{
	TestPrefsManager m(kSignature, 0);
	TestPrefObject o1(&m, 5);
	TestPrefObject o2(&m, 8);

	o1.JPrefObject::ReadPrefs();		// empty
	o2.JPrefObject::ReadPrefs();

	o1.JPrefObject::WritePrefs();
	o2.JPrefObject::WritePrefs();

	o1.JPrefObject::ReadPrefs();
	o2.JPrefObject::ReadPrefs();
}

JTEST(OldFile)
{
	TestPrefsManager m(kSignature, 1);	// upgrade version
	TestPrefObject o1(&m, 5);
	TestPrefObject o2(&m, 8);

	o1.JPrefObject::ReadPrefs();
	o2.JPrefObject::ReadPrefs();
}

JTEST(CleanUp)
{
	JString fileName("~/.");
	fileName += kSignature;
	fileName += ".pref";

	JString fullName;
	JAssertTrue(JExpandHomeDirShortcut(fileName, &fullName));
	JAssertOK(JRemoveFile(fullName));
}