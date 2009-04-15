/******************************************************************************
 MDMDIServer.cc

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright © 2008 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include <mdStdInc.h>
#include "MDMDIServer.h"
#include "mdGlobals.h"
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::MDMDIServer";

/******************************************************************************
 Constructor

 *****************************************************************************/

MDMDIServer::MDMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDMDIServer::~MDMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
MDMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
MDMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers",      MDGetVersionNumberStr(),
		"copyright", JGetString("COPYRIGHT")
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
