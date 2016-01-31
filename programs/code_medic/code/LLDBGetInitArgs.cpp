/******************************************************************************
 LLDBGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .lldbinit

	BASE CLASS = CMGetInitArgs

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetInitArgs.h"
#include "cmGlobals.h"
#include <JXInputField.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetInitArgs::LLDBGetInitArgs
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

LLDBGetInitArgs::~LLDBGetInitArgs()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

	Use the value from the result.  Otherwise, grab everything after the
	first \" and toss the trailing \"\.

 ******************************************************************************/

void
LLDBGetInitArgs::HandleSuccess
	(
	const JString& data
	)
{
/*
	const JString& result = GetLastResult();

	JArray<JIndexRange> matchList;
	JIndex index;
	if (resultPattern.Match(result, &matchList))
		{
		JString args = result.GetSubstring(matchList.GetElement(2));
		LLDBScanner::TranslateMIOutput(&args);
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
		(CMGetLink())->Log("LLDBGetInitArgs failed to match");
		}
*/
}
