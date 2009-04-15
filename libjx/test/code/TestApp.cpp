/******************************************************************************
 TestApp.cpp

	BASE CLASS = JXApplication

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestApp.h"
#include "TestDirector.h"
#include "AboutDialog.h"
#include "TestMDIServer.h"
#include "testjxStringData.h"
#include "testjxGlobals.h"
#include <JString.h>
#include <JThisProcess.h>
#include <JOutPipeStream.h>
#include <jSysUtil.h>
#include <jAssert.h>

// Application signature (MDI)

static const JCharacter* kAppSignature = "testjx";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestApp::TestApp
	(
	int*			argc,
	char*			argv[],
	const JBoolean	wantMDI
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
	itsMainDirector = NULL;

	TestjxCreateGlobals(this, wantMDI);
	ListenTo(JThisProcess::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestApp::~TestApp()
{
	TestjxDeleteGlobals();
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
TestApp::DisplayAbout
	(
	JXDisplay* display
	)
{
	AboutDialog* dlog = new AboutDialog(this);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
TestApp::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason == JXDocumentManager::kServerDead)
		{
		const JString userName = JGetUserName();

		const JCharacter* argv[] = { "sendmail", userName, NULL };

		JProcess* p;
		int toFD;
		JError err = JProcess::Create(&p, argv, sizeof(argv), kJCreatePipe, &toFD);
		if (err.OK())
			{
			JOutPipeStream output(toFD, kJTrue);
			output << "Subject: server crash" << endl << endl;
			output << "We're sorry, but the X Windows server crashed while" << endl;
			output << "you were running testjx.  (Or had you already noticed?)" << endl;
			output << "." << endl;
			}
		}

	TestjxDeleteGlobals();
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	This tests the signal handlers in JThisProcess.

 ******************************************************************************/

void
TestApp::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == JThisProcess::Instance())
		{
		cout << "Received signal: " << message->GetType() << endl;
		}

	JXApplication::ReceiveWithFeedback(sender, message);
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
TestApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
TestApp::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kDefaultStringData);
}
