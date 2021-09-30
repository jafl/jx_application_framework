/******************************************************************************
 App.cpp

	BASE CLASS = public JXApplication

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#include "App.h"
#include "AboutDialog.h"
#include "stringData.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "<Binary>";

/******************************************************************************
 Constructor

 *****************************************************************************/

App::App
	(
	int*		argc,
	char*		argv[],
	bool*		displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
	*displayAbout = CreateGlobals(this);

	if (!*displayAbout)
	{
		*prevVersStr = GetPrefsManager()->GetPrevVersionStr();
		if (*prevVersStr == GetVersionNumberStr())
		{
			prevVersStr->Clear();
		}
		else
		{
			*displayAbout = true;
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

App::~App()
{
	DeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
App::DisplayAbout
	(
	const JString& prevVersStr
	)
{
	AboutDialog* dlog = jnew AboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
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

	if (reason != JXDocumentManager::kAssertFired)
	{
//		JPrefObject::WritePrefs();
	}

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
