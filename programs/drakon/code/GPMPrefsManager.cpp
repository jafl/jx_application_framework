/******************************************************************************
 GPMPrefsManager.cc

	<Description>

	BASE CLASS = public JXPrefsManager

	Copyright © 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include "GPMPrefsManager.h"
#include "gpmGlobals.h"
#include <JXChooseSaveFile.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMPrefsManager::GPMPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kGPMgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMPrefsManager::~GPMPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (private)

 ******************************************************************************/

void
GPMPrefsManager::SaveAllBeforeDestruct()
{
	(JXGetChooseSaveFile())->JPrefObject::WritePrefs();

	SetData(kGPMProgramVersionID, GPMGetVersionNumberStr());

	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
GPMPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kGPMProgramVersionID, GPMGetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
GPMPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(kGPMProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by GPMCleanUpBeforeSuddenDeath()
	so we can save the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
GPMPrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveAllBeforeDestruct();
		}
}
