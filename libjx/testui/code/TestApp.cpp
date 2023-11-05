/******************************************************************************
 TestApp.cpp

	BASE CLASS = JXApplication

	Written by John Lindal.

 ******************************************************************************/

#include "TestApp.h"
#include "TestDirector.h"
#include "AboutDialog.h"
#include "TestMDIServer.h"
#include "TestTextEditDocument.h"
#include "stringData.h"
#include "testjxGlobals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <fstream>
#include <jx-af/jcore/jAssert.h>

// Application signature (MDI)

static const JUtf8Byte* kAppSignature = "testjx";

static const JUtf8Byte* kOpenFilesFileName = "testjx_open_files";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestApp::TestApp
	(
	int*			argc,
	char*			argv[],
	const bool	wantMDI
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
	itsMainDirector = nullptr;

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
 OpenDocuments

	Re-open TestTextEditDocuments.

 ******************************************************************************/

void
TestApp::OpenDocuments()
{
	if (!JFileExists(JString(kOpenFilesFileName, JString::kNoCopy)))
	{
		return;
	}

	std::ifstream input(kOpenFilesFileName);

	JSize count;
	input >> count;

	JString fullName;
	for (JIndex i=1; i<=count; i++)
	{
		input >> fullName;

		if (JFileExists(fullName))
		{
			TestTextEditDocument* doc = jnew TestTextEditDocument(this, fullName, false);
			doc->GetWindow()->ReadGeometry(input);
			doc->Activate();
		}
	}
}

/******************************************************************************
 Close (virtual)

	Save all open TestTextEditDocuments.

 ******************************************************************************/

bool
TestApp::Close()
{
	JXDocumentManager* mgr = JXGetDocumentManager();
	JXDocument* doc;

	JIndex i = 1;
	JSize count = 0;
	while (mgr->GetDocument(i, &doc))
	{
		TestTextEditDocument* teDoc = dynamic_cast<TestTextEditDocument*>(doc);
		if (teDoc != nullptr && teDoc->ExistsOnDisk())
		{
			count++;
		}
		i++;
	}

	std::ofstream output(kOpenFilesFileName);

	output << count;

	i = 1;
	while (mgr->GetDocument(i, &doc))
	{
		TestTextEditDocument* teDoc = dynamic_cast<TestTextEditDocument*>(doc);
		if (teDoc != nullptr)
		{
			bool onDisk;
			const JString fullName = teDoc->GetFullName(&onDisk);
			if (onDisk)
			{
				output << ' ' << fullName;
				output << ' ';
				teDoc->GetWindow()->WriteGeometry(output);
			}
		}
		i++;
	}

	return JXApplication::Close();
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
	AboutDialog* dlog = jnew AboutDialog;
	dlog->DoDialog();
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

		const JUtf8Byte* argv[] = { "sendmail", userName.GetBytes(), nullptr };

		JProcess* p;
		int toFD;
		JError err = JProcess::Create(&p, argv, sizeof(argv), kJCreatePipe, &toFD);
		if (err.OK())
		{
			JOutPipeStream output(toFD, true);
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
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}
