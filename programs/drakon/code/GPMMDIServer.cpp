/******************************************************************************
 GPMMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 2001 by Glenn W. Bach.

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
	itsMainDirector(nullptr)
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
	if (itsMainDirector != nullptr)
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
	std::cout << std::endl << s << std::endl << std::endl;
}
