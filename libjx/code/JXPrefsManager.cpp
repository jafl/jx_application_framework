/******************************************************************************
 JXPrefsManager.cpp

	Builds the file name based on the application signature.

	BASE CLASS = JPrefsManager

	Copyright (C) 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXPrefsManager.h>
#include <JXChooseSaveFile.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <jAssert.h>

const Time kSafetySaveInterval = 15 * 60 * 1000;	// 15 minutes (ms)

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPrefsManager::JXPrefsManager
	(
	const JFileVersion	currentVersion,
	const JBoolean		eraseFileIfOpen
	)
	:
	JPrefsManager((JXGetApplication())->GetSignature(),
				  currentVersion, eraseFileIfOpen)
{
	itsSafetySaveTask = jnew JXTimerTask(kSafetySaveInterval);
	assert( itsSafetySaveTask != NULL );
	itsSafetySaveTask->Start();
	ListenTo(itsSafetySaveTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPrefsManager::~JXPrefsManager()
{
	jdelete itsSafetySaveTask;
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
	(JXGetChooseSaveFile())->JPrefObject::WritePrefs();
	SaveToDisk();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSafetySaveTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		SaveAllBeforeDestruct();
		}
	else
		{
		JPrefsManager::Receive(sender, message);
		}
}
