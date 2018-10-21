/******************************************************************************
 TestPrefsManager.cpp

	BASE CLASS = JMDIServer

	Written by John Lindal.

 ******************************************************************************/

#include "TestPrefsManager.h"
#include <JTestManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPrefsManager::TestPrefsManager
	(
	const JUtf8Byte*	signature,
	const JFileVersion	currentVersion
	)
	:
	JPrefsManager(JString(signature, kJFalse), currentVersion, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPrefsManager::~TestPrefsManager()
{
	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
TestPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		JAssertTrue(IsEmpty());
		}
	else
		{
		JAssertEqual(0, currentVersion);
		}
}
