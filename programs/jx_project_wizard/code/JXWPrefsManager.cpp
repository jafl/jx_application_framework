/******************************************************************************
 JXWPrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

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
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
JXWPrefsManager::SaveAllBeforeDestruct()
{
	SetData(kJXWProgramVersionID, JXWGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
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
