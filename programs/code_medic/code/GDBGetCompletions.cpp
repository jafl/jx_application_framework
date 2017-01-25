/******************************************************************************
 GDBGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GDBGetCompletions.h"
#include "CMCommandInput.h"
#include "CMCommandOutputDisplay.h"
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetCompletions::GDBGetCompletions
	(
	CMCommandInput*			input,
	CMCommandOutputDisplay*	history
	)
	:
	CMGetCompletions(BuildCommand(input->GetText())),
	itsPrefix(input->GetText()),
	itsInput(input),
	itsHistory(history)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetCompletions::~GDBGetCompletions()
{
}

/******************************************************************************
 BuildCommand (static private)

 ******************************************************************************/

JString
GDBGetCompletions::BuildCommand
	(
	const JCharacter* prefix
	)
{
	JString cmd = "complete ";
	cmd        += prefix;
	return cmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetCompletions::HandleSuccess
	(
	const JString& data
	)
{
	JPtrArray<JString> lines(JPtrArrayT::kDeleteAll);
	lines.SetSortOrder(JOrderedSetT::kSortAscending);
	lines.SetCompareFunction(JCompareStringsCaseSensitive);

	// loop through each line and add each one to our list

	JIndex i = 1, j = 1;
	while (data.LocateNextSubstring("\n", &j))
		{
		if (j > i)
			{
			JString* s = jnew JString(data, JIndexRange(i, j-1));
			assert( s != NULL );
			s->TrimWhitespace();
			if (s->IsEmpty() || !lines.InsertSorted(s, kJFalse))
				{
				jdelete s;
				}
			}
		i = j+1;
		j = i;
		}

	if (i <= data.GetLength())
		{
		JString* s = jnew JString(data, JIndexRange(i, data.GetLength()));
		assert( s != NULL );
		s->TrimWhitespace();
		if (s->IsEmpty() || !lines.InsertSorted(s, kJFalse))
			{
			jdelete s;
			}
		}

	if (lines.IsEmpty())
		{
		(itsInput->GetDisplay())->Beep();
		return;
		}

	// Check if we're done. If we find our test prefix in the array, and
	// the array has only one element, we're done.  Otherwise, our test
	// prefix is the 'start' of several possible commands.

	const JSize stringCount = lines.GetElementCount();
	JBoolean found;
	JIndex startIndex = lines.SearchSorted1(&itsPrefix, JOrderedSetT::kAnyMatch, &found);
	if (found)
		{
		if (stringCount == 1)
			{
			// The input text is already complete.  We just need to add a
			// space at the end to show that it is a complete word.

			itsPrefix += " ";
			}
		else
			{
			// We can't add anything to what the user has types, so we show
			// all possible completions.

			itsHistory->PlaceCursorAtEnd();
			itsHistory->Paste("\n");
			itsHistory->Paste(data);
			}
		itsInput->SetText(itsPrefix);
		itsInput->SetCaretLocation(itsInput->GetTextLength() + 1);
		return;
		}

	JString maxPrefix;

	maxPrefix = *(lines.GetElement(startIndex));
	if (stringCount == 1)
		{
		// There's only one completion, which must be what we meant so we
		// fill in the input with this word.

		maxPrefix += " ";
		itsInput->SetText(maxPrefix);
		itsInput->SetCaretLocation(itsInput->GetTextLength() + 1);
		return;
		}

	for (JIndex i=startIndex+1; i<=stringCount; i++)
		{
		const JString* s = lines.GetElement(i);
		const JSize matchLength  = JCalcMatchLength(maxPrefix, *s);
		const JSize prefixLength = maxPrefix.GetLength();
		if (matchLength < prefixLength)
			{
			maxPrefix.RemoveSubstring(matchLength+1, prefixLength);
			}
		}

	if (maxPrefix == itsPrefix)
		{
		// The input text is all that the words have in common so we can't
		// add anything to the input. We therefore need to dump everything
		// to the history window.

		itsHistory->PlaceCursorAtEnd();
		itsHistory->Paste("\n");
		itsHistory->Paste(data);
		}
	else
		{
		itsInput->SetText(maxPrefix);
		itsInput->SetCaretLocation(itsInput->GetTextLength() + 1);
		}
}
