/******************************************************************************
 GPMMDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#include "GPMMDIServer.h"
#include "GPMMainDirector.h"
#include "gpmGlobals.h"
#include <jAssert.h>

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
	const JString&				dir,
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
	const JUtf8Byte* map[] =
		{
		"version",   GPMGetVersionNumberStr().GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	const JString s = JGetString("CommandLineHelp::GPMMDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
