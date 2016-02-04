/******************************************************************************
 GDBVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBVarCommand.h"
#include "GDBVarTreeParser.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBVarCommand::GDBVarCommand
	(
	const JCharacter* origCmd
	)
	:
	CMVarCommand()
{
	JString cmd = "set print pretty off\nset print array off\n"
				  "set print repeats 0\nset width 0\n";
	cmd        += origCmd;
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBVarCommand::~GDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static JRegex prefixPattern =
	"^([^\v]*\v\n*|warning:[^\n\v]*[\n\v]+)*[[:space:]]*"
	"\\$[[:digit:]]+[[:space:]]*=[[:space:]]*";

void
GDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	JString s = data;
	s.TrimWhitespace();

	JBoolean success = kJFalse;

	JIndexRange r;
	prefixPattern.SetSingleLine();
	if (prefixPattern.Match(s, &r))
		{
		s.RemoveSubstring(r);
		SetData(s);

		GDBVarTreeParser parser(s);
		if (parser.yyparse() == 0)
			{
			parser.ReportRecoverableError();
			success = kJTrue;
			Broadcast(ValueMessage(kValueUpdated, parser.GetRootNode()));
			}
		}
	else
		{
		(CMGetLink())->Log("GDBVarCommand failed to match");
		}

	if (!success)
		{
		Broadcast(ValueMessage(kValueFailed, NULL));
		}

	s.Clear();
	SetData(s);
}
