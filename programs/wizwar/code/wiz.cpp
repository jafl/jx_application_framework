/******************************************************************************
 wiz.cpp

	Graphical WizWar client.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "WizApp.h"
#include "wwMessages.h"
#include "wizGlobals.h"
#include <jCommandLine.h>
#include <stdlib.h>
#include <jAssert.h>

// Prototypes

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
void PrintVersion();

// string ID's

static const JCharacter* kCommandLineHelpID = "CommandLineHelp::wizMain";

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(argc, argv);

	WizApp* app = new WizApp(&argc, argv);
	assert( app != NULL );

	app->Run();
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions() won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
	while (index < argc)
		{
		if (JIsVersionRequest(argv[index]))
			{
			WizApp::InitStrings();
			PrintVersion();
			exit(0);
			}
		else if (JIsHelpRequest(argv[index]))
			{
			WizApp::InitStrings();
			PrintHelp();
			exit(0);
			}
		index++;
		}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	const JString protVers(kWWCurrentProtocolVersion, 0);
	const JCharacter* map[] =
		{
		"vers",          WizGetVersionNumberStr(),
		"protocol_vers", protVers
		};
	const JString s = JGetString(kCommandLineHelpID, map, sizeof(map));
	cout << endl << s << endl << endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	cout << endl;
	cout << WizGetVersionStr() << endl;
	cout << endl;
}

/******************************************************************************
 Required templates

	These are defined because the linker asked for them.

 ******************************************************************************/

// must undef these before including ACE headers

#undef new
#undef delete

#include "WWSocket.h"
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/Version.h>

// INET

#define JTemplateName ACE_Connector
#define JTemplateType WWSocket, ACE_SOCK_CONNECTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#if ACE_MAJOR_VERSION > 5 || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION >= 4) || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION == 4 && ACE_BETA_VERSION >= 4)

#define JTemplateName ACE_NonBlocking_Connect_Handler
#define JTemplateType WWSocket
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Connector_Base
#define JTemplateType WWSocket
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#else

#define JTemplateName ACE_Map_Manager
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<WWSocket>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<WWSocket>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Reverse_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<WWSocket>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator_Base
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<WWSocket>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Entry
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<WWSocket>*
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Svc_Tuple
#define JTemplateType WWSocket
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

// generic

#define JTemplateName ACE_Read_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Write_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#endif
