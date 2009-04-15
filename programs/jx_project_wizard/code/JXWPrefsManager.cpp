/******************************************************************************
 JXWPrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <jxwStdInc.h>
#include "JXWPrefsManager.h"
#include "jxwGlobals.h"
#include <JXChooseSaveFile.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

JXWPrefsManager::JXWPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kJXWgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXWPrefsManager::~JXWPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (private)

 ******************************************************************************/

void
JXWPrefsManager::SaveAllBeforeDestruct()
{
	(JXGetChooseSaveFile())->JPrefObject::WritePrefs();

	SetData(kJXWProgramVersionID, JXWGetVersionNumberStr());

	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
JXWPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kJXWProgramVersionID, JXWGetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
JXWPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(kJXWProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by JXWCleanUpBeforeSuddenDeath()
	so we can save the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
JXWPrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveAllBeforeDestruct();
		}
}
