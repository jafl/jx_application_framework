/******************************************************************************
 GFGApp.cpp

	<Description>

	BASE CLASS = public JXApplication

	Copyright (C) 2002 by New Planet Software.

 *****************************************************************************/

#include "GFGApp.h"
#include "GFGAboutDialog.h"
#include "gfgStringData.h"
#include "gfgGlobals.h"
#include <jAssert.h>

static const JCharacter* kAppSignature = "forge";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGApp::GFGApp
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kGFGDefaultStringData),
	itsDeletingTemplate(kJFalse)
{
	*displayAbout = GFGCreateGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = (GFGGetPrefsManager())->GetPrevVersionStr();
		if (*prevVersStr == GFGGetVersionNumberStr())
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

GFGApp::~GFGApp()
{
	GFGDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
GFGApp::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	GFGAboutDialog* dlog = jnew GFGAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
GFGApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
//		JPrefObject::WritePrefs();
		}

	GFGCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 IsDeletingTemplate (public)

 ******************************************************************************/

JBoolean
GFGApp::IsDeletingTemplate()
	const
{
	return itsDeletingTemplate;
}

/******************************************************************************
 ShouldBeDeletingTemplate (public)

 ******************************************************************************/

void
GFGApp::ShouldBeDeletingTemplate
	(
	const JBoolean delTemplate
	)
{
	itsDeletingTemplate	= delTemplate;
}


/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
GFGApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
GFGApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kGFGDefaultStringData);
}
