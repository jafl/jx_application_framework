/******************************************************************************
 MDApp.cc

	BASE CLASS = public JXApplication

	Copyright © 2008 by John Lindal. All rights reserved.

 *****************************************************************************/

#include "MDApp.h"
#include "MDAboutDialog.h"
#include "mdStringData.h"
#include "mdGlobals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "mondrian";

/******************************************************************************
 Constructor

 *****************************************************************************/

MDApp::MDApp
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kMDDefaultStringData)
{
	*displayAbout = MDCreateGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = (MDGetPrefsManager())->GetPrevVersionStr();
		if (*prevVersStr == MDGetVersionNumberStr())
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

MDApp::~MDApp()
{
	MDDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be NULL.

 ******************************************************************************/

void
MDApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	MDAboutDialog* dlog = new MDAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
MDApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
//		JPrefObject::WritePrefs();
		}

	MDCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
MDApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
MDApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kMDDefaultStringData);
}
