/******************************************************************************
 CMApp.cpp

	BASE CLASS = JXApplication

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CMApp.h"
#include "CMMDIServer.h"
#include "CMAboutDialog.h"
#include "CMQuitTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include "cmStringData.h"
#include <JXWindow.h>
#include <JXAskInitDockAll.h>
#include <JSimpleProcess.h>
#include <JSubstitute.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// Application signature (MDI, prefs)

static const JUtf8Byte* kAppSignature = "medic";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMApp::CMApp
	(
	int*		argc,
	char*		argv[],
	bool*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kCMDefaultStringData)
{
	*displayAbout = CMCreateGlobals(this);
	if (!*displayAbout)
		{
		*prevVersStr = CMGetPrefsManager()->GetMedicVersionStr();
		if (*prevVersStr == CMGetVersionNumberStr())
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

	auto* mdi = jnew CMMDIServer;		// deleted by jXGlobals
	assert( mdi != nullptr );

	auto* task = jnew CMQuitTask();
	assert( task != nullptr );
	task->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMApp::~CMApp()
{
	CMDeleteGlobals();
}

/******************************************************************************
 EditFile

	Tell text editor to display the specified file.
	If lineIndex > 0, displays the specified line of the file.

 ******************************************************************************/

void
CMApp::EditFile
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
	const
{
	JString editFileCmd, editFileLineCmd;
	CMGetPrefsManager()->GetEditFileCmds(&editFileCmd, &editFileLineCmd);

	JString cmd;
	if (lineIndex > 0)
		{
		cmd = editFileLineCmd;
		}
	else
		{
		cmd = editFileCmd;
		}

	const JString name = JPrepArgForExec(fileName);
	const JString lineIndexStr(lineIndex, 0);

	JSubstitute sub;
	sub.IgnoreUnrecognized();
	sub.DefineVariable("f", name);
	sub.DefineVariable("l", lineIndexStr);
	sub.Substitute(&cmd);

	JSimpleProcess::Create(cmd, true);
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
CMApp::DisplayAbout
	(
	const JString&	prevVersStr,
	const bool	init
	)
{
	auto* dlog = jnew CMAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();

	if (init && prevVersStr.IsEmpty())
		{
		auto* task = jnew JXAskInitDockAll(dlog);
		assert( task != nullptr );
		task->Start();
		}
}

/******************************************************************************
 HandleCustomEvent (virtual protected)

 ******************************************************************************/

bool
CMApp::HandleCustomEvent()
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->HandleLLDBEvent();
		}
	return true;
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CMApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);
	CMCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

	Required for call to WillBeMDIServer() *before* app is constructed.

 ******************************************************************************/

const JUtf8Byte*
CMApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static public)

 ******************************************************************************/

void
CMApp::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kCMDefaultStringData);
}
