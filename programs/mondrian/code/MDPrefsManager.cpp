/******************************************************************************
 MDPrefsManager.cc

	BASE CLASS = public JXPrefsManager

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <mdStdInc.h>
#include "MDPrefsManager.h"
#include "MDPrefsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXChooseSaveFile.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

MDPrefsManager::MDPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue),
	itsPrefsDialog(NULL)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kMDgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDPrefsManager::~MDPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (private)

 ******************************************************************************/

void
MDPrefsManager::SaveAllBeforeDestruct()
{
	(JXGetChooseSaveFile())->JPrefObject::WritePrefs();

	SetData(kMDProgramVersionID, MDGetVersionNumberStr());

	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
MDPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kMDProgramVersionID, MDGetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
MDPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(kMDProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
MDPrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == NULL );

	// replace with whatever is appropriate
	JString data;

	itsPrefsDialog = new MDPrefsDialog(JXGetApplication(), data);
	assert( itsPrefsDialog != NULL );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

JBoolean
MDPrefsManager::GetWindowSize
	(
	const JPrefID&	id,
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	std::string data;
	if (GetData(id, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *desktopLoc >> *width >> *height;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
MDPrefsManager::SaveWindowSize
	(
	const JPrefID&	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	data << window->GetDesktopLocation();
	data << ' ' << window->GetFrameWidth();
	data << ' ' << window->GetFrameHeight();

	SetData(id, data);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MDPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			// replace with whatever is appropriate
			JString data;

			itsPrefsDialog->GetValues(&data);

			// store data somehow
			}
		itsPrefsDialog = NULL;
		}

	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by MDCleanUpBeforeSuddenDeath()
	so we can save the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
MDPrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveAllBeforeDestruct();
		}
}
