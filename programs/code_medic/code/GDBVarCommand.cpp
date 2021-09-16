/******************************************************************************
 GDBVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBVarCommand.h"
#include "GDBVarTreeParser.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBVarCommand::GDBVarCommand
	(
	const JString& origCmd
	)
	:
	CMVarCommand()
{
	JString cmd("set print pretty off\nset print array off\n"
				"set print repeats 0\nset width 0\n");
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

	bool success = false;

	prefixPattern.SetSingleLine();

	JStringIterator iter(&s);
	if (iter.Next(prefixPattern))
		{
		iter.RemoveAllPrev();
		iter.Invalidate();
		SetData(s);

		GDBVarTreeParser parser;
		if (parser.Parse(s) == 0)
			{
			parser.ReportRecoverableError();
			success = true;
			Broadcast(ValueMessage(kValueUpdated, parser.GetRootNode()));
			}
		}
	else
		{
		CMGetLink()->Log("GDBVarCommand failed to match");
		}

	if (!success)
		{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		}

	s.Clear();
	SetData(s);
}
