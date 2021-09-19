/******************************************************************************
 GDBGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = CMGetInitArgs

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include "GDBGetInitArgs.h"
#include "GDBOutputScanner.h"
#include "cmGlobals.h"
#include <JXInputField.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetInitArgs::GDBGetInitArgs
	(
	JXInputField* argInput
	)
	:
	CMGetInitArgs(JString("show args", JString::kNoCopy)),
	itsArgInput(argInput)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetInitArgs::~GDBGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

	Use the value from the result.  Otherwise, grab everything after the
	first \" and toss the trailing \"\.

 ******************************************************************************/

static const JRegex resultPattern = "done,value=\"(.*)\"";

void
GDBGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
	const JStringMatch m = resultPattern.Match(GetLastResult(), JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		JString args = m.GetSubstring(1);
		GDB::Output::Scanner::TranslateMIOutput(&args);
		itsArgInput->GetText()->SetText(args);
		return;
		}

	JStringIterator iter(data);
	if (iter.Next("\"") && !iter.AtEnd())
		{
		iter.BeginMatch();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		if (!iter.Prev("\"."))
			{
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			}

		JString args = iter.FinishMatch().GetString();
		args.TrimWhitespace();
		itsArgInput->GetText()->SetText(args);
		return;
		}

	CMGetLink()->Log("GDBGetInitArgs failed to match");
}
