/******************************************************************************
 GDBDisplaySourceForMain.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = CMDisplaySourceForMain

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBDisplaySourceForMain.h"
#include "CMSourceDirector.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

static const JCharacter* kCommand[] =
{
	"info line main",
	"info line _start",
	"info line MAIN__"
};

static const JCharacter* kBreakCommand[] =
{
	"tbreak main",
	"tbreak _start",
	"tbreak MAIN__"
};

const JSize kCommandCount = sizeof(kCommand) / sizeof(JCharacter*);

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBDisplaySourceForMain::GDBDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
	:
	CMDisplaySourceForMain(sourceDir, kCommand[0]),
	itsHasCoreFlag(kJFalse),
	itsNextCmdIndex(1)
{
	ListenTo(CMGetLink());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBDisplaySourceForMain::~GDBDisplaySourceForMain()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GDBDisplaySourceForMain::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CMGetLink() && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		itsHasCoreFlag = (CMGetLink())->HasCore();
		if (info->Successful())
			{
			itsNextCmdIndex = 1;
			SetCommand(kCommand[0]);
			CMCommand::Send();
			}
		else if (!itsHasCoreFlag)
			{
			(GetSourceDir())->ClearDisplay();
			}
		}
	else
		{
		CMDisplaySourceForMain::Receive(sender, message);
		}
}

/******************************************************************************
 HandleSuccess (virtual protected)

	Look for the special format provided by using --fullname option.

 ******************************************************************************/

static const JRegex infoPattern =
	"Line [[:digit:]]+ of \"[^\"]*\" starts at address";
static const JRegex locationPattern =
	"032032(.+):([[:digit:]]+):[[:digit:]]+:[^:]+:0x[[:xdigit:]]+";

void
GDBDisplaySourceForMain::HandleSuccess
	(
	const JString& data
	)
{
	if (infoPattern.Match(data))
		{
		JArray<JIndexRange> matchList;
		if (locationPattern.Match(data, &matchList))
			{
			const JString fileName = data.GetSubstring(matchList.GetElement(2));

			const JString lineStr = data.GetSubstring(matchList.GetElement(3));
			JIndex lineIndex;
			const JBoolean ok = lineStr.ConvertToUInt(&lineIndex);
			assert( ok );

			if (!itsHasCoreFlag)
				{
				(GetSourceDir())->DisplayFile(fileName, lineIndex, kJFalse);
				}
			}

		const JCharacter* map[] =
			{
			"tbreak_cmd", kBreakCommand[ itsNextCmdIndex-1 ]
			};
		const JString cmd = JGetString("RunCommand::GDBDisplaySourceForMain", map, sizeof(map));
		dynamic_cast<GDBLink*>(CMGetLink())->SendWhenStopped(cmd);
		}
	else if (itsNextCmdIndex < kCommandCount)
		{
		SetCommand(kCommand [ itsNextCmdIndex ]);
		itsNextCmdIndex++;
		CMCommand::Send();
		}
	else
		{
		(CMGetLink())->Log("GDBDisplaySourceForMain failed to match");

		if (!itsHasCoreFlag)
			{
			(GetSourceDir())->ClearDisplay();
			}

		dynamic_cast<GDBLink*>(CMGetLink())->FirstBreakImpossible();

		const JCharacter* map[] =
			{
			"tbreak_cmd", ""
			};
		const JString cmd = JGetString("RunCommand::GDBDisplaySourceForMain", map, sizeof(map));
		dynamic_cast<GDBLink*>(CMGetLink())->SendWhenStopped(cmd);
		}
}
