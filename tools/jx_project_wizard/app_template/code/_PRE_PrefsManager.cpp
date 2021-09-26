/******************************************************************************
 <PRE>PrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#include "<PRE>PrefsManager.h"
#include "<PRE>PrefsDialog.h"
#include "<pre>Globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

<PRE>PrefsManager::<PRE>PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true),
	itsPrefsDialog(nullptr)
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
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
<PRE>PrefsManager::SaveAllBeforeDestruct()
{
	SetData(k<PRE>ProgramVersionID, <PRE>GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
<PRE>PrefsManager::UpgradeData
	(
	const bool		isNew,
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
	const bool ok = GetData(k<PRE>ProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
<PRE>PrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == nullptr );

	// replace with whatever is appropriate
	JString data;

	itsPrefsDialog = jnew <PRE>PrefsDialog(JXGetApplication(), data);
	assert( itsPrefsDialog != nullptr );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

bool
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
		return true;
	}
	else
	{
		return false;
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
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			// replace with whatever is appropriate
			JString data;

			itsPrefsDialog->GetValues(&data);

			// store data somehow
		}
		itsPrefsDialog = nullptr;
	}

	else
	{
		JXPrefsManager::Receive(sender, message);
	}
}
