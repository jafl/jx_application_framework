/******************************************************************************
 TestMDIServer.cpp

	BASE CLASS = JXMDIServer

	Written by John Lindal.

 ******************************************************************************/

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
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	JUserNotification* un = JGetUserNotification();

	const JString origDir = JGetCurrentDirectory();
	if (JChangeDirectory(dir) != kJNoError)
		{
		const JUtf8Byte* map[] =
			{
			"dir", dir.GetBytes()
			};
		un->ReportError(JGetString("DirNotReadable::TestMDIServer", map, sizeof(map)));
		return;
		}

	TestDirector* director = NULL;
	const JBoolean ok = (TestjxGetApplication())->GetMainDirector(&director);
	assert( ok );

	const JSize argCount = argList.GetElementCount();
	if (argCount == 1)
		{
		un->DisplayMessage(JGetString("NoArgs::TestMDIServer"));
		}
	else
		{
		JXDocumentManager* docMgr = JXGetDocumentManager();

		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(argCount-1, JGetString("OpeningFiles::TestMDIServer"), kJTrue, kJFalse);

		for (JIndex i=2; i<=argCount; i++)
			{
			const JString& fileName = *(argList.GetElement(i));

			JXFileDocument* doc;
			if (docMgr->FileDocumentIsOpen(fileName, &doc))
				{
				doc->Activate();
				}
			else
				{
				if (!JFileExists(fileName))
					{
					const JUtf8Byte* map[] =
						{
						"name", fileName.GetBytes()
						};
					if (!un->AskUserYes(JGetString("FileNotFound::TestMDIServer", map, sizeof(map))))
						{
						continue;
						}
					std::ofstream temp(fileName.GetBytes());
					if (!temp.good())
						{
						un->ReportError(JGetString("CannotCreateFile::TestMDIServer"));
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
	std::cout << std::endl << JGetString("CommandLineHelp::TestMDIServer") << std::endl << std::endl;
}
