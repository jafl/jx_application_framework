/******************************************************************************
 TestMDIServer.cpp

	BASE CLASS = JXMDIServer

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestMDIServer.h"
#include "TestApp.h"
#include "TestDirector.h"
#include "testjxGlobals.h"
#include <JXDocumentManager.h>
#include <JXFileDocument.h>
#include <JXStandAlonePG.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::TestMDIServer";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestMDIServer::TestMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestMDIServer::~TestMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 ******************************************************************************/

void
TestMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	JUserNotification* un = JGetUserNotification();

	const JString origDir = JGetCurrentDirectory();
	if (JChangeDirectory(dir) != kJNoError)
		{
		JString msg = "Unable to access \"";
		msg += dir;
		msg += "\".";
		un->ReportError(msg);
		return;
		}

	TestDirector* director = NULL;
	const JBoolean ok = (TestjxGetApplication())->GetMainDirector(&director);
	assert( ok );

	const JSize argCount = argList.GetElementCount();
	if (argCount == 1)
		{
		un->DisplayMessage("testjx received MDI with no arguments.");
		}
	else
		{
		JXDocumentManager* docMgr = JXGetDocumentManager();

		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(argCount-1, "Opening files...", kJTrue, kJFalse);

		for (JIndex i=2; i<=argCount; i++)
			{
			const JString& fileName = *(argList.NthElement(i));

			JXFileDocument* doc;
			if (docMgr->FileDocumentIsOpen(fileName, &doc))
				{
				doc->Activate();
				}
			else
				{
				if (!JFileExists(fileName))
					{
					JString msg = "\"";
					msg += fileName;
					msg += "\" does not exist.  Do you want to create it?";
					if (!un->AskUserYes(msg))
						{
						continue;
						}
					ofstream temp(fileName);
					if (!temp.good())
						{
						un->ReportError("Unable to create it.  "
							"Please check that the directory is writable.");
						continue;
						}
					}

				director->OpenTextFile(fileName);
				}

			if (!pg.IncrementProgress())
				{
				break;
				}
			}

		pg.ProcessFinished();
		}

	const JError err = JChangeDirectory(origDir);
	assert_ok( err );
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
TestMDIServer::PrintCommandLineHelp()
{
	cout << endl << JGetString(kCommandLineHelpID) << endl << endl;
}
