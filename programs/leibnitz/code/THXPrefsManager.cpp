/******************************************************************************
 THXPrefsManager.cpp

	Exists to keep JCheckForNewerVersion() happy.

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "THXPrefsManager.h"
#include "thxGlobals.h"
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

THXPrefsManager::THXPrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true)
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
	const bool		isNew,
	const JFileVersion	currentVersion
	)
{
}
