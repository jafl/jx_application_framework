/******************************************************************************
 App.cpp

	BASE CLASS = public JXApplication

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#include "App.h"
#include "stringData.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "jxwizard";

/******************************************************************************
 Constructor

 *****************************************************************************/

App::App
	(
	int*	argc,
	char*	argv[],
	bool*	displayLicense
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
	*displayLicense = CreateGlobals(this);

	if (!*displayLicense)
	{
		*displayLicense = GetPrefsManager()->GetPrevVersionStr() != GetVersionNumberStr();
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

App::~App()
{
	DeleteGlobals();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
App::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);
/*
	if (reason != JXDocumentManager::kAssertFired)
	{
	}
*/
	::CleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
App::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
App::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}
