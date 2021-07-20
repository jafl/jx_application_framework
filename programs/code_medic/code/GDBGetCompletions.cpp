/******************************************************************************
 GDBGetCompletions.cpp

	BASE CLASS = CMGetCompletions

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#include "GDBGetCompletions.h"
#include "CMCommandInput.h"
#include "CMCommandOutputDisplay.h"
#include <JXDisplay.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
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
	CMGetCompletions(BuildCommand(input->GetText()->GetText())),
	itsPrefix(input->GetText()->GetText()),
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
	const JString& prefix
	)
{
	return "complete " + prefix;
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
	lines.SetSortOrder(JListT::kSortAscending);
	lines.SetCompareFunction(JCompareStringsCaseSensitive);

	// loop through each line and add each one to our list

	JStringIterator iter(data);
	iter.BeginMatch();
	while (iter.Next("\n"))
		{
		const JStringMatch& m = iter.FinishMatch();
		if (!m.IsEmpty())
			{
			auto* s = jnew JString(m.GetString());
			assert( s != nullptr );
			s->TrimWhitespace();
			if (s->IsEmpty() || !lines.InsertSorted(s, false))
				{
				jdelete s;
				}
			}

		iter.BeginMatch();
		}

	if (!iter.AtEnd())
		{
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		const JStringMatch& m = iter.FinishMatch();

		auto* s = jnew JString(m.GetString());
		assert( s != nullptr );
		s->TrimWhitespace();
		if (s->IsEmpty() || !lines.InsertSorted(s, false))
			{
			jdelete s;
			}
		}
	iter.Invalidate();

	if (lines.IsEmpty())
		{
		itsInput->GetDisplay()->Beep();
		return;
		}

	// Check if we're done. If we find our test prefix in the array, and
	// the array has only one element, we're done.  Otherwise, our test
	// prefix is the 'start' of several possible commands.

	const JSize stringCount = lines.GetElementCount();
	bool found;
	JIndex startIndex = lines.SearchSorted1(&itsPrefix, JListT::kAnyMatch, &found);
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
			itsHistory->Paste(JString::newline);
			itsHistory->Paste(data);
			}
		itsInput->GetText()->SetText(itsPrefix);
		itsInput->GoToEndOfLine();
		return;
		}

	JString maxPrefix;

	maxPrefix = *(lines.GetElement(startIndex));
	if (stringCount == 1)
		{
		// There's only one completion, which must be what we meant so we
		// fill in the input with this word.

		maxPrefix += " ";
		itsInput->GetText()->SetText(maxPrefix);
		itsInput->GoToEndOfLine();
		return;
		}

	for (JIndex i=startIndex+1; i<=stringCount; i++)
		{
		const JString* s         = lines.GetElement(i);
		const JSize matchLength  = JString::CalcCharacterMatchLength(maxPrefix, *s);
		const JSize prefixLength = maxPrefix.GetCharacterCount();
		if (matchLength < prefixLength)
			{
			maxPrefix = JString(maxPrefix, JCharacterRange(1, matchLength));
			}
		}

	if (maxPrefix == itsPrefix)
		{
		// The input text is all that the words have in common so we can't
		// add anything to the input. We therefore need to dump everything
		// to the history window.

		itsHistory->PlaceCursorAtEnd();
		itsHistory->Paste(JString::newline);
		itsHistory->Paste(data);
		}
	else
		{
		itsInput->GetText()->SetText(maxPrefix);
		itsInput->GoToEndOfLine();
		}
}
