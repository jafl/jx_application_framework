/******************************************************************************
 <PRE>MDIServer.cc

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>MDIServer.h"
#include "<pre>Globals.h"
#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::<PRE>MDIServer";

/******************************************************************************
 Constructor

 *****************************************************************************/

<PRE>MDIServer::<PRE>MDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

<PRE>MDIServer::~<PRE>MDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
<PRE>MDIServer::HandleMDIRequest
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
<PRE>MDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers", <PRE>GetVersionNumberStr()
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
