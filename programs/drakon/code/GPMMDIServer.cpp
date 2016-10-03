/******************************************************************************
 GPMMDIServer.cc

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright (C) 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include "GPMMDIServer.h"
#include "GPMMainDirector.h"
#include "gpmGlobals.h"
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::GPMMDIServer";

/******************************************************************************
 Constructor

 *****************************************************************************/

GPMMDIServer::GPMMDIServer()
	:
	JXMDIServer(),
	itsMainDirector(NULL)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMMDIServer::~GPMMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
GPMMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	if (itsMainDirector != NULL)
		{
		itsMainDirector->Activate();
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
GPMMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"version",   GPMGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
