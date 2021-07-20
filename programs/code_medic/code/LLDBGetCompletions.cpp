/******************************************************************************
 LLDBGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetCompletions.h"
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBStringList.h"
#include "CMCommandInput.h"
#include "CMCommandOutputDisplay.h"
#include "cmGlobals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
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
	CMGetCompletions(JString::empty),
	itsPrefix(input->GetText()->GetText()),
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
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBCommandInterpreter interp = link->GetDebugger()->GetCommandInterpreter();
	if (!interp.IsValid())
		{
		return;
		}

	lldb::SBStringList matches;
	const JSize matchCount = interp.HandleCompletion(itsPrefix.GetBytes(), itsPrefix.GetByteCount(), 0, -1, matches);

	if (matchCount == 0)
		{
		itsInput->GetDisplay()->Beep();
		return;
		}
	else if (strlen(matches.GetStringAtIndex(0)) > 0)
		{
		itsInput->GoToEndOfLine();
		itsInput->Paste(JString(matches.GetStringAtIndex(0), JString::kNoCopy));
		return;
		}
	else if (matchCount == 1)
		{
		itsInput->GetText()->SetText(JString(matches.GetStringAtIndex(1), JString::kNoCopy));
		itsInput->GoToEndOfLine();
		return;
		}

	itsHistory->PlaceCursorAtEnd();
	itsHistory->Paste(JString::newline);

	const JSize count = matches.GetSize();
	for (JIndex i=1; i<count; i++)
		{
		itsHistory->Paste(JString(matches.GetStringAtIndex(i), JString::kNoCopy));
		itsHistory->Paste(JString::newline);
		}
}
