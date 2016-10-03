/******************************************************************************
 GDBGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = CMGetInitArgs

	Copyright (C) 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetInitArgs.h"
#include "GDBScanner.h"
#include "cmGlobals.h"
#include <JXInputField.h>
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
	CMGetInitArgs("show args"),
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
	const JString& result = GetLastResult();

	JArray<JIndexRange> matchList;
	JIndex index;
	if (resultPattern.Match(result, &matchList))
		{
		JString args = result.GetSubstring(matchList.GetElement(2));
		GDBScanner::TranslateMIOutput(&args);
		itsArgInput->SetText(args);
		}
	else if (data.LocateSubstring("\"", &index) && index < data.GetLength())
		{
		JString args = data.GetSubstring(index+1, data.GetLength());
		args.TrimWhitespace();
		if (args.EndsWith("\"."))
			{
			args.RemoveSubstring(args.GetLength()-1, args.GetLength());
			}
		itsArgInput->SetText(args);
		}
	else
		{
		(CMGetLink())->Log("GDBGetInitArgs failed to match");
		}
}
