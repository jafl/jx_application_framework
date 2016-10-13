/******************************************************************************
 GFGMDIServer.cpp

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright (C) 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include "GFGMDIServer.h"
#include "GFGMainDirector.h"
#include "GFGApp.h"
#include "gfgGlobals.h"

#include <JXChooseSaveFile.h>

#include <jFileUtil.h>

#include <jAssert.h>

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::GFGMDIServer";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGMDIServer::GFGMDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGMDIServer::~GFGMDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
GFGMDIServer::HandleMDIRequest
	(
	const JCharacter*			dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize count = argList.GetElementCount();
	if (count <= 1)
		{
		JGetUserNotification()->ReportError("Please specify the template file to load.");
		return;
		}

	for (JIndex i = 2; i <= count; i++)
		{
		JString arg	= *(argList.NthElement(i));
		if (arg == "--delete")
			{
			GFGGetApplication()->ShouldBeDeletingTemplate(kJTrue);
			}
		}

	GFGMainDirector* dialog = jnew GFGMainDirector(JXGetApplication(), argList);
	assert( dialog != NULL );
	dialog->Activate();

	if (GFGGetApplication()->IsDeletingTemplate())
		{
		for (JIndex i = 2; i <= count; i++)
			{
			JString arg	= *(argList.NthElement(i));
			if (!arg.BeginsWith("-"))
				{
				JRemoveFile(arg);
				}
			}		
		}
}

/******************************************************************************
 PrintCommandLineHelp (static)

 ******************************************************************************/

void
GFGMDIServer::PrintCommandLineHelp()
{
	const JCharacter* map[] =
		{
		"vers", GFGGetVersionNumberStr()
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}
