/******************************************************************************
 MDApp.cpp

	BASE CLASS = public JXApplication

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDApp.h"
#include "MDAboutDialog.h"
#include "mdStringData.h"
#include "mdGlobals.h"
#include <JSimpleProcess.h>
#include <JSubstitute.h>
#include <JMemoryManager.h>
#include <jAssert.h>

static const JUtf8Byte* kAppSignature = "jx_memory_debugger";

/******************************************************************************
 Constructor

 *****************************************************************************/

MDApp::MDApp
	(
	int*		argc,
	char*		argv[],
	bool*	displayAbout,
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

MDApp::~MDApp()
{
	MDDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
MDApp::DisplayAbout
	(
	const JString& prevVersStr
	)
{
	auto* dlog = jnew MDAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 OpenFile

 ******************************************************************************/

void
MDApp::OpenFile
	(
	const JString&	fileName,
	const JSize		lineIndex
	)
{
	if (fileName == JMemoryManager::kUnknownFile)
		{
		return;
		}

	JString cmd = (MDGetPrefsManager())->GetOpenFileCommand();

	JString lineStr((JUInt64) lineIndex);

	JSubstitute subst;
	subst.DefineVariable("f", fileName);
	subst.DefineVariable("l", lineStr);
	subst.Substitute(&cmd);

	JSimpleProcess::Create(cmd, true);
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

const JUtf8Byte*
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
	JGetStringManager()->Register(kAppSignature, kMDDefaultStringData);
}
