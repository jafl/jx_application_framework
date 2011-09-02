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
