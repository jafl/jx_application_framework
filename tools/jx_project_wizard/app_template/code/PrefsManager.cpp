/******************************************************************************
 PrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#include "PrefsManager.h"
#include "PrefsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

PrefsManager::PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true, kgCSFSetupID)
{
	*isNew = JPrefsManager::UpgradeData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PrefsManager::~PrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
PrefsManager::SaveAllBeforeDestruct()
{
	SetData(kProgramVersionID, GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
PrefsManager::UpgradeData
	(
	const bool			isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
	{
		SetData(kProgramVersionID, GetVersionNumberStr());
	}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
PrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const bool ok = GetData(kProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
PrefsManager::EditPrefs()
{
	// replace with whatever is appropriate
	JString data;

	auto* dlog = jnew PrefsDialog(data);

	if (dlog->DoDialog())
	{
		// replace with whatever is appropriate
		dlog->GetValues(&data);

		// store data somehow
	}
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

bool
PrefsManager::GetWindowSize
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
PrefsManager::SaveWindowSize
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
