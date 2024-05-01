/******************************************************************************
 JXWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Firefox) to display the data.

	BASE CLASS = JWebBrowser, JXSharedPrefObject

	Copyright (C) 2000-10 by John Lindal.

 ******************************************************************************/

#include "JXWebBrowser.h"
#include "JXEditWWWPrefsDialog.h"
#include "JXSharedPrefsManager.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

static const JXSharedPrefObject::VersionInfo kVersList[] =
{
	JXSharedPrefObject::VersionInfo(0, JXSharedPrefsManager::kWebBrowserV0),
	JXSharedPrefObject::VersionInfo(1, JXSharedPrefsManager::kWebBrowserV1)
};

const JSize kVersCount = sizeof(kVersList) / sizeof(JXSharedPrefObject::VersionInfo);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWebBrowser::JXWebBrowser()
	:
	JXSharedPrefObject(GetCurrentConfigVersion(),
					   JXSharedPrefsManager::kLatestWebBrowserVersionID,
					   kVersList, kVersCount),
	itsSaveChangesFlag(true)
{
	JXSharedPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWebBrowser::~JXWebBrowser()
{
}

/******************************************************************************
 SaveCommands (virtual protected)

 ******************************************************************************/

void
JXWebBrowser::SaveCommands()
{
	if (itsSaveChangesFlag)
	{
		JXSharedPrefObject::WritePrefs();
	}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
JXWebBrowser::EditPrefs()
{
	auto* dlog = jnew JXEditWWWPrefsDialog(GetShowURLCmd(),
										   GetShowFileContentCmd(),
										   GetShowFileLocationCmd(),
										   GetComposeMailCmd());
	if (dlog->DoDialog())
	{
		JString showURLCmd, showFileContentCmd, showFileLocationCmd, composeMailCmd;
		dlog->GetPrefs(&showURLCmd, &showFileContentCmd,
					   &showFileLocationCmd, &composeMailCmd);

		itsSaveChangesFlag = false;
		SetShowURLCmd(showURLCmd);
		SetShowFileContentCmd(showFileContentCmd);
		SetShowFileLocationCmd(showFileLocationCmd);
		SetComposeMailCmd(composeMailCmd);
		itsSaveChangesFlag = true;

		SaveCommands();
	}
}

/******************************************************************************
 ReadPrefs (virtual)

	We assert that we can read the given data because there is no
	way to skip over it.

 ******************************************************************************/

void
JXWebBrowser::ReadPrefs
	(
	std::istream& input
	)
{
	ReadConfig(input);
}

/******************************************************************************
 WritePrefs (virtual)

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JXWebBrowser::WritePrefs
	(
	std::ostream&			output,
	const JFileVersion	vers
	)
	const
{
	WriteConfig(output, vers);
}
