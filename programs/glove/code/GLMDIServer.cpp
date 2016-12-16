/******************************************************************************
 GLMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <GLMDIServer.h>
#include <GLGlobals.h>
#include <GLPrefsMgr.h>
#include <GLPlotApp.h>

#include <JXStandAlonePG.h>
#include <jXGlobals.h>

#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GLMDIServer::GLMDIServer
	(
	GLPlotApp* app
	)
	:
	JXMDIServer()
{
	itsApp = app;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLMDIServer::~GLMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
GLMDIServer::HandleMDIRequest
	(
	const JCharacter* dir,
	const JPtrArray<JString>& argList
	)
{
	const JSize argCount = argList.GetElementCount();
	if (argCount == 1)
		{
//		GNBGetPrefsMgr()->RestoreProgramState();
		itsApp->NewFile();
//		GNBGetPrefsMgr()->ForgetProgramState();
		return;
		}
	else
		{
//		GNBGetPrefsMgr()->ForgetProgramState();
		}

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

	JXStandAlonePG pg;
	pg.RaiseWhenUpdate();
	if (argCount > 4)
		{
		pg.FixedLengthProcessBeginning(argCount-1, "Opening files...", kJTrue, kJFalse);
		}

	for (JIndex i=2; i<=argCount; i++)
		{
		const JString& fileName = *(argList.NthElement(i));
		if (fileName.IsEmpty())
			{
			continue;
			}

		JBoolean shouldOpen = kJTrue;

		const JBoolean isFile = JFileExists(fileName);
		if (!isFile && JNameUsed(fileName))
			{
			JString msg = "\"";
			msg += fileName;
			msg += "\" is not a regular file, so it cannot be opened.";
			un->ReportError(msg);
			shouldOpen = kJFalse;
			}
		else if (!isFile)
			{
			shouldOpen = kJFalse;
/*			JString msg = "\"";
			msg += fileName;
			msg += "\" does not exist.  Do you want to create it?";
			if (!un->AskUserYes(msg))
				{
				shouldOpen = kJFalse;
				}
			else
				{
				std::ofstream temp(fileName);
				if (!temp.good())
					{
					un->ReportError("Unable to create it.  "
						"Please check that the directory is writable.");
					shouldOpen = kJFalse;
					}
				}*/
			}

		if (shouldOpen)
			{
			itsApp->OpenFile(fileName);
			}

		if (pg.ProcessRunning() && !pg.IncrementProgress())
			{
			break;
			}
		}

	if (pg.ProcessRunning())
		{
		pg.ProcessFinished();
		}

	const JError err = JChangeDirectory(origDir);
	assert_ok( err );
}
