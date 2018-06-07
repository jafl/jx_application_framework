/******************************************************************************
 GPMApp.cc

	<Description>

	BASE CLASS = public JXApplication

	Copyright (C) 2001 by New Planet Software.

 *****************************************************************************/

#include "GPMApp.h"
#include "GPMAboutDialog.h"
#include "gpmStringData.h"
#include "gpmGlobals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "npsprocessmgr";

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMApp::GPMApp
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kGPMDefaultStringData)
{
	*displayAbout = GPMCreateGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = (GPMGetPrefsManager())->GetPrevVersionStr();
		if (*prevVersStr == GPMGetVersionNumberStr())
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

GPMApp::~GPMApp()
{
	GPMDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
GPMApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	GPMAboutDialog* dlog = jnew GPMAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
GPMApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
//		JPrefObject::WritePrefs();
		}

	GPMCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
GPMApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
GPMApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kGPMDefaultStringData);
}
