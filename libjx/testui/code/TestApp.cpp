/******************************************************************************
 TestApp.cpp

	BASE CLASS = JXApplication

	Written by John Lindal.

 ******************************************************************************/

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

static const JUtf8Byte* kAppSignature = "testjx";

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
	AboutDialog* dlog = jnew AboutDialog(this);
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

		const JUtf8Byte* argv[] = { "sendmail", userName.GetBytes(), NULL };

		JProcess* p;
		int toFD;
		JError err = JProcess::Create(&p, argv, sizeof(argv), kJCreatePipe, &toFD);
		if (err.OK())
			{
			JOutPipeStream output(toFD, kJTrue);
			output << "Subject: server crash" << std::endl << std::endl;
			output << "We're sorry, but the X Windows server crashed while" << std::endl;
			output << "you were running testjx.  (Or had you already noticed?)" << std::endl;
			output << "." << std::endl;
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
		std::cout << "Received signal: " << message->GetType() << std::endl;
		}

	JXApplication::ReceiveWithFeedback(sender, message);
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
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
