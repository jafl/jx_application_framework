/******************************************************************************
 JXPrefsManager.cpp

	Builds the file name based on the application signature.

	BASE CLASS = JPrefsManager

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#include "JXPrefsManager.h"
#include "JXCSFDialogBase.h"
#include "JXFunctionTask.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const Time kSafetySaveInterval = 15 * 60 * 1000;	// 15 minutes (ms)

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPrefsManager::JXPrefsManager
	(
	const JFileVersion	currentVersion,
	const bool			eraseFileIfOpen,
	const JPrefID&		csfID
	)
	:
	JPrefsManager(JXGetApplication()->GetSignature(),
				  currentVersion, eraseFileIfOpen),
	itsCSFPrefID(csfID)
{
	itsSafetySaveTask = jnew JXFunctionTask(kSafetySaveInterval, [this]()
	{
		SaveAllBeforeDestruct();	// virtual
	},
	"JXPrefsManager::SafetySave");
	itsSafetySaveTask->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPrefsManager::~JXPrefsManager()
{
	jdelete itsSafetySaveTask;
}

/******************************************************************************
 DataLoaded (virtual protected)

 ******************************************************************************/

void
JXPrefsManager::DataLoaded()
{
	std::string data;
	if (itsCSFPrefID.IsValid() && GetData(itsCSFPrefID, &data))
	{
		JXCSFDialogBase::SetState(JString(data));
	}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual)

	This must be the last one called by the application's
	CleanUpBeforeSuddenDeath() function so we can save the preferences to
	disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
JXPrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
	{
		SaveAllBeforeDestruct();
	}
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

	This must be called by the leaf class destructor.

 ******************************************************************************/

void
JXPrefsManager::SaveAllBeforeDestruct()
{
	if (itsCSFPrefID.IsValid())
	{
		SetData(itsCSFPrefID, JXCSFDialogBase::GetState());
	}
	SaveToDisk();
}
