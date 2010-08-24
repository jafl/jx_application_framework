/******************************************************************************
 <PRE>PrefsManager.cc

	BASE CLASS = public JXPrefsManager

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>PrefsManager.h"
#include "<PRE>PrefsDialog.h"
#include "<pre>Globals.h"
#include <JXWindow.h>
#include <JXChooseSaveFile.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

<PRE>PrefsManager::<PRE>PrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue),
	itsPrefsDialog(NULL)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, k<PRE>gCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

<PRE>PrefsManager::~<PRE>PrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (private)

 ******************************************************************************/

void
<PRE>PrefsManager::SaveAllBeforeDestruct()
{
	(JXGetChooseSaveFile())->JPrefObject::WritePrefs();

	SetData(k<PRE>ProgramVersionID, <PRE>GetVersionNumberStr());

	SaveToDisk();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
<PRE>PrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(k<PRE>ProgramVersionID, <PRE>GetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
<PRE>PrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(k<PRE>ProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
<PRE>PrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == NULL );

	// replace with whatever is appropriate
	JString data;

	itsPrefsDialog = new <PRE>PrefsDialog(JXGetApplication(), data);
	assert( itsPrefsDialog != NULL );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

JBoolean
<PRE>PrefsManager::GetWindowSize
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
<PRE>PrefsManager::SaveWindowSize
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
<PRE>PrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
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

	This must be the last one called by <PRE>CleanUpBeforeSuddenDeath()
	so we can save the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
<PRE>PrefsManager::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		SaveAllBeforeDestruct();
		}
}
