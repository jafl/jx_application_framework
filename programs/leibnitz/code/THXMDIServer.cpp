/******************************************************************************
 THXMDIServer.cpp

	BASE CLASS = JXMDIServer

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXMDIServer.h"
#include "thxGlobals.h"
#include <JXDisplay.h>
#include <jAssert.h>

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
	const JString&				dir,
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
	const JUtf8Byte* map[] =
		{
		"vers", THXGetVersionNumberStr().GetBytes()
		};
	JString s = JGetString("CommandLineHelp::THXMDIServer");
	JGetStringManager()->Replace(&s, map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
