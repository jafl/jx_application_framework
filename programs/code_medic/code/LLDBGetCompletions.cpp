/******************************************************************************
 LLDBGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetCompletions.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBStringList.h"
#include "CMCommandInput.h"
#include "CMCommandOutputDisplay.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetCompletions::LLDBGetCompletions
	(
	CMCommandInput*			input,
	CMCommandOutputDisplay*	history
	)
	:
	CMGetCompletions(""),
	itsPrefix(input->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetCompletions::~LLDBGetCompletions()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
		{
		return;
		}

	lldb::SBStringList matches;
	const JSize matchCount = interp.HandleCompletion(itsPrefix, itsPrefix.GetLength(), 0, -1, matches);

	if (matchCount == 0)
		{
		(itsInput->GetDisplay())->Beep();
		return;
		}
	else if (strlen(matches.GetStringAtIndex(0)) > 0)
		{
		itsInput->SetCaretLocation(itsInput->GetTextLength() + 1);
		itsInput->Paste(matches.GetStringAtIndex(0));
		return;
		}
	else if (matchCount == 1)
		{
		itsInput->SetText(matches.GetStringAtIndex(1));
		itsInput->SetCaretLocation(itsInput->GetTextLength() + 1);
		return;
		}

	itsHistory->PlaceCursorAtEnd();
	itsHistory->Paste("\n");

	const JSize count = matches.GetSize();
	for (JIndex i=1; i<count; i++)
		{
		itsHistory->Paste(matches.GetStringAtIndex(i));
		itsHistory->Paste("\n");
		}
}
