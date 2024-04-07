/******************************************************************************
 TestMDIServer.cpp

	BASE CLASS = JXMDIServer

	Written by John Lindal.

 ******************************************************************************/

#include "TestMDIServer.h"
#include "TestApp.h"
#include "TestDirector.h"
#include "testjxGlobals.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXStandAlonePG.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

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
	if (JChangeDirectory(dir))
	{
		const JUtf8Byte* map[] =
		{
			"dir", dir.GetBytes()
		};
		un->ReportError(JGetString("DirNotReadable::TestMDIServer", map, sizeof(map)));
		return;
	}

	TestDirector* director = nullptr;
	bool ok = (TestjxGetApplication())->GetMainDirector(&director);
	assert( ok );

	const JSize argCount = argList.GetItemCount();
	if (argCount == 1)
	{
		un->DisplayMessage(JGetString("NoArgs::TestMDIServer"));
	}
	else
	{
		JXDocumentManager* docMgr = JXGetDocumentManager();

		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(argCount-1, JGetString("OpeningFiles::TestMDIServer"), true, true);

		for (JIndex i=2; i<=argCount; i++)
		{
			const JString& fileName = *argList.GetItem(i);

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

	ok = JChangeDirectory(origDir);
	assert( ok );
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
TestMDIServer::PrintCommandLineHelp()
{
	std::cout << std::endl << JGetString("CommandLineHelp::TestMDIServer") << std::endl << std::endl;
}
