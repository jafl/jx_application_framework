/******************************************************************************
 JXWApp.cpp

	BASE CLASS = public JXApplication

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <jxwStdInc.h>
#include "JXWApp.h"
#include "jxwStringData.h"
#include "jxwGlobals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "jxwizard";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXWApp::JXWApp
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayLicense
	)
	:
	JXApplication(argc, argv, kAppSignature, kJXWDefaultStringData)
{
	*displayLicense = JXWCreateGlobals(this);

	if (!*displayLicense)
		{
		*displayLicense = JI2B(
			(JXWGetPrefsManager())->GetPrevVersionStr() != JXWGetVersionNumberStr());
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXWApp::~JXWApp()
{
	JXWDeleteGlobals();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
JXWApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		}

	JXWCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
JXWApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
JXWApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kJXWDefaultStringData);
}
