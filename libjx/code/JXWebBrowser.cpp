/******************************************************************************
 JXWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Firefox) to display the data.

	BASE CLASS = JWebBrowser, JXSharedPrefObject

	Copyright (C) 2000-10 by John Lindal.

 ******************************************************************************/

#include <JXWebBrowser.h>
#include <JXEditWWWPrefsDialog.h>
#include <JXSharedPrefsManager.h>
#include <jXGlobals.h>
#include <jAssert.h>

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
	itsPrefsDialog(nullptr),
	itsSaveChangesFlag(kJTrue)
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
	assert( itsPrefsDialog == nullptr );

	itsPrefsDialog = jnew JXEditWWWPrefsDialog(JXGetApplication(), GetShowURLCmd(),
											  GetShowFileContentCmd(),
											  GetShowFileLocationCmd(),
											  GetComposeMailCmd());
	assert( itsPrefsDialog != nullptr );
	itsPrefsDialog->BeginDialog();
	ListenTo(itsPrefsDialog);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXWebBrowser::Receive
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
			JString showURLCmd, showFileContentCmd, showFileLocationCmd, composeMailCmd;
			itsPrefsDialog->GetPrefs(&showURLCmd, &showFileContentCmd,
									 &showFileLocationCmd, &composeMailCmd);

			itsSaveChangesFlag = kJFalse;
			SetShowURLCmd(showURLCmd);
			SetShowFileContentCmd(showFileContentCmd);
			SetShowFileLocationCmd(showFileLocationCmd);
			SetComposeMailCmd(composeMailCmd);
			itsSaveChangesFlag = kJTrue;

			SaveCommands();
			}
		itsPrefsDialog = nullptr;
		}

	else
		{
		JXSharedPrefObject::Receive(sender, message);
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
