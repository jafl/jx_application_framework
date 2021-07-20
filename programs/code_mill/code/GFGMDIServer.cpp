/******************************************************************************
 GFGMDIServer.cpp

	<Description>

	BASE CLASS = public JXMDIServer

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#include "GFGMDIServer.h"
#include "GFGMainDirector.h"
#include "GFGApp.h"
#include "gfgGlobals.h"
#include <JXChooseSaveFile.h>
#include <jFileUtil.h>
#include <jAssert.h>

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
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	const JSize count = argList.GetElementCount();
	if (count <= 1)
		{
		JGetUserNotification()->ReportError(JGetString("MissingTemplate::GFGMDIServer"));
		return;
		}

	for (JIndex i = 2; i <= count; i++)
		{
		JString arg	= *(argList.GetElement(i));
		if (arg == "--delete")
			{
			GFGGetApplication()->ShouldBeDeletingTemplate(true);
			}
		}

	auto* dialog = jnew GFGMainDirector(JXGetApplication(), argList);
	assert( dialog != nullptr );
	dialog->Activate();

	if (GFGGetApplication()->IsDeletingTemplate())
		{
		for (JIndex i=2; i <= count; i++)
			{
			const JString& arg	= *(argList.GetElement(i));
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
	const JUtf8Byte* map[] =
		{
		"vers", GFGGetVersionNumberStr().GetBytes()
		};
	const JString s = JGetString("CommandLineHelp::GFGMDIServer", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}
