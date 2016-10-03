/******************************************************************************
 THXMDIServer.cpp

	BASE CLASS = JXMDIServer

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXMDIServer.h"
#include "thxGlobals.h"
#include <JXDisplay.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::THXMDIServer";

/******************************************************************************
 Constructor

 ******************************************************************************/

THXMDIServer::THXMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXMDIServer::~THXMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest (virtual protected)

 ******************************************************************************/

void
THXMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	((THXGetApplication())->GetCurrentDisplay())->RaiseAllWindows();
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
THXMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers", THXGetVersionNumberStr()
		};
	JString s = JGetString(kCommandLineHelpID);
	(JGetStringManager())->Replace(&s, map, sizeof(map));
	cout << endl << s << endl << endl;
}
