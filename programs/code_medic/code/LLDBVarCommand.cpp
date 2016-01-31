/******************************************************************************
 LLDBVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBVarCommand.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBVarCommand::LLDBVarCommand
	(
	const JCharacter* origCmd
	)
	:
	CMVarCommand()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBVarCommand::~LLDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
/*
	JString s = data;
	s.TrimWhitespace();

	JBoolean success = kJFalse;

	JIndexRange r;
	prefixPattern.SetSingleLine();
	if (prefixPattern.Match(s, &r))
		{
		s.RemoveSubstring(r);
		SetData(s);

		LLDBVarTreeParser parser(s);
		if (parser.yyparse() == 0)
			{
			parser.ReportRecoverableError();
			success = kJTrue;
			Broadcast(ValueMessage(kValueUpdated, parser.GetRootNode()));
			}
		}
	else
		{
		(CMGetLink())->Log("LLDBVarCommand failed to match");
		}

	if (!success)
		{
		Broadcast(ValueMessage(kValueFailed, NULL));
		}

	s.Clear();
	SetData(s);
*/
}
