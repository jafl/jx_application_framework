/******************************************************************************
 WizApp.cpp

	BASE CLASS = JXApplication

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "WizApp.h"
#include "WizChatDirector.h"
#include "WizConnectDialog.h"
#include "wizGlobals.h"
#include "wizFileVersions.h"
#include "wwUtil.h"
#include "wwMessages.h"
#include "wwStringData.h"
#include <JThisProcess.h>
#include <jSysUtil.h>
#include <stdlib.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WizApp::WizApp
	(
	int*	argc,
	char*	argv[]
	)
	:
	JXApplication(argc, argv, WWGetAppSignature(), kWWDefaultStringData)
{
	itsChatDirector  = NULL;
	itsConnectDialog = NULL;

	itsPlayerName = JGetUserRealWorldName();

	WizCreateGlobals(this);
	Connect(*argc, argv);
	ReadProgramPrefs();
}

/******************************************************************************
 Destructor

	Delete global objects in reverse order of creation.

 ******************************************************************************/

WizApp::~WizApp()
{
	WriteProgramPrefs();
	WizDeleteGlobals();
}

/******************************************************************************
 Connect (private)

 ******************************************************************************/

void
WizApp::Connect
	(
	const JSize	argc,
	char*		argv[]
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();
	ListenTo(connMgr);

	if (argc < 3)
		{
		if (argc == 2)
			{
			itsServerAddr = argv[1];
			}
		RequestConnectInfo();
		}
	else
		{
		itsServerAddr = argv[1];
		itsPlayerName = argv[2];
		connMgr->Connect(itsServerAddr, itsPlayerName);
		}
}

/******************************************************************************
 RequestConnectInfo (private)

 ******************************************************************************/

void
WizApp::RequestConnectInfo()
{
	assert( itsConnectDialog == NULL );

	itsConnectDialog = new WizConnectDialog(this, itsServerAddr, itsPlayerName);
	assert( itsConnectDialog != NULL );
	itsConnectDialog->BeginDialog();
	ListenTo(itsConnectDialog);
}

/******************************************************************************
 ProcessConnectInfo (private)

 ******************************************************************************/

void
WizApp::ProcessConnectInfo()
{
	assert( itsConnectDialog != NULL );

	itsConnectDialog->GetConnectInfo(&itsServerAddr, &itsPlayerName);
	(WizGetConnectionManager())->Connect(itsServerAddr, itsPlayerName);
}

/******************************************************************************
 NotifyConnectionRefused (private)

	Returns kJTrue if we should try again.

 ******************************************************************************/

JBoolean
WizApp::NotifyConnectionRefused
	(
	const Wiz2War::ConnectionRefused& info
	)
{
	JBoolean retry = kJFalse;

	JString msg;

	const WWConnectionErrorType err = info.GetError();
	if (err == kWWWrongProtocolVersion)
		{
		msg = "You need a different version of this program in order to play.\n\n"
			  "The protocol version of this program and the server must be the same.  "
			  "This is printed when you run them on the command line with --version.";
		}
	else if (err == kWWBadClient)
		{
		msg = "The WizWar server did not understand the information it received.  "
			  "The program you are using may be damaged.";
		}
	else if (err == kWWPlayerNameUsed)
		{
		msg = "The name you chose is already being used by another player.  "
			  "Please try again.";
		retry = kJTrue;
		}
	else if (err == kWWGameFull)
		{
		msg = "You cannot join the game because it is already full.";
		}
	else
		{
		msg  = "Unknown error #";
		msg += JString(err, JString::kBase10);
		msg += " received from server.";
		}

	(JGetUserNotification())->ReportError(msg);
	return retry;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WizApp::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();

	if (sender == connMgr && message.Is(Wiz2War::kConnectionAccepted))
		{
		itsChatDirector->Activate();
		}

	else if (sender == connMgr && message.Is(Wiz2War::kConnectionRefused))
		{
		const Wiz2War::ConnectionRefused* info =
			dynamic_cast(const Wiz2War::ConnectionRefused*, &message);
		assert( info != NULL );
		if (NotifyConnectionRefused(*info))
			{
			RequestConnectInfo();
			}
		else
			{
			Quit();
			}
		}

	else if (sender == connMgr && message.Is(Wiz2War::kDisconnect))
		{
		(JGetUserNotification())->ReportError(
			"You have been disconnected from the WizWar server.");
		Quit();
		}

	else if (sender == itsConnectDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			ProcessConnectInfo();
			}
		else
			{
			Quit();
			}
		itsConnectDialog = NULL;
		}

	else
		{
		JXApplication::Receive(sender, message);
		}
}

/******************************************************************************
 ReadProgramPrefs (private)

 ******************************************************************************/

void
WizApp::ReadProgramPrefs()
{
	itsChatDirector = new WizChatDirector(this);
	assert( itsChatDirector != NULL );
}

/******************************************************************************
 WriteProgramPrefs (private)

 ******************************************************************************/

void
WizApp::WriteProgramPrefs()
{
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
WizApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	(WizGetConnectionManager())->CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		WriteProgramPrefs();
		}
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
WizApp::InitStrings()
{
	(JGetStringManager())->Register(WWGetAppSignature(), kWWDefaultStringData);
}
