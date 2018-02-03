/******************************************************************************
 SVNApp.cc

	<Description>

	BASE CLASS = public JXApplication

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "SVNApp.h"
#include "SVNAboutDialog.h"
#include "svnStringData.h"
#include "svnGlobals.h"
#include <JSimpleProcess.h>
#include <jAssert.h>

static const JCharacter* kAppSignature = "nps_svn_client";

/******************************************************************************
 Constructor

 *****************************************************************************/

SVNApp::SVNApp
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kSVNDefaultStringData)
{
	*displayAbout = SVNCreateGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = (SVNGetPrefsManager())->GetPrevVersionStr();
		if (*prevVersStr == SVNGetVersionNumberStr())
			{
			prevVersStr->Clear();
			}
		else
			{
			*displayAbout = kJTrue;
			}
		}
	else
		{
		prevVersStr->Clear();
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SVNApp::~SVNApp()
{
	SVNDeleteGlobals();
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

JBoolean
SVNApp::Close()
{
	(SVNGetPrefsManager())->SaveProgramState();

	const JBoolean success = JXApplication::Close();	// deletes us if successful
	if (!success)
		{
		(SVNGetPrefsManager())->ForgetProgramState();
		}

	return success;
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be NULL.

 ******************************************************************************/

void
SVNApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	SVNAboutDialog* dlog = jnew SVNAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 ReloadOpenFilesInIDE

 ******************************************************************************/

void
SVNApp::ReloadOpenFilesInIDE()
{
	SVNPrefsManager::Integration type;
	JString cmd;
	if ((SVNGetPrefsManager())->GetCommand(SVNPrefsManager::kReloadChangedCmd, &type, &cmd))
		{
		JSimpleProcess::Create(cmd, kJTrue);
		}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
SVNApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		(SVNGetPrefsManager())->SaveProgramState();
		}

	SVNCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
SVNApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
SVNApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kSVNDefaultStringData);
}
