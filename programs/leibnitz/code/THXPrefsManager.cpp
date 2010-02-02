/******************************************************************************
 THXPrefsManager.cc

	Exists to keep JCheckForNewerVersion() happy.

	BASE CLASS = public JXPrefsManager

	Copyright © 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <thxStdInc.h>
#include "THXPrefsManager.h"
#include "thxGlobals.h"
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

THXPrefsManager::THXPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	*isNew = JPrefsManager::UpgradeData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

THXPrefsManager::~THXPrefsManager()
{
	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
THXPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by MDCleanUpBeforeSuddenDeath()
	so we can save the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
THXPrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveToDisk();
		}
}
