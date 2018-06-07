/******************************************************************************
 CBMacroSubstitute.cpp

	BASE CLASS = JSubstitute

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#include "CBMacroSubstitute.h"
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <JString.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CBMacroSubstitute::CBMacroSubstitute()
	:
	JSubstitute()
{
	itsExecCount = 0;
	TurnOnEscapes();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CBMacroSubstitute::~CBMacroSubstitute()
{
}

/******************************************************************************
 Evaluate (virtual protected)

	Override to handle evaluation of commands:  $(w $($(x) y) z))

 *****************************************************************************/

JBoolean
CBMacroSubstitute::Evaluate
	(
	const JString&	s,
	const JIndex	startIndex,
	JIndexRange*	matchRange,
	JString*		value
	)
	const
{
	if (s.GetCharacter(startIndex) == '(')
		{
		if (GetExecRange(s, startIndex, matchRange))
			{
			*value = s.GetSubstring(startIndex+1, matchRange->last-1);

			CBMacroSubstitute* me = const_cast<CBMacroSubstitute*>(this);
			if (itsExecCount == 0)
				{
				me->TurnOffEscapes();
				}
			me->itsExecCount++;
			Substitute(value);		// recursive!
			me->itsExecCount--;
			if (itsExecCount == 0)
				{
				me->TurnOnEscapes();
				}

			pid_t pid;
			int fromFD, errFD;
			const JError execErr =
				JExecute(*value, &pid, kJIgnoreConnection, nullptr,
						 kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
			if (!execErr.OK())
				{
				execErr.ReportIfError();
				value->Clear();
				}
			else
				{
				JString msg;
				JReadAll(errFD, &msg);
				if (!msg.IsEmpty())
					{
					msg.Prepend("Error occurred:\n\n");
					(JGetUserNotification())->ReportError(msg);
					}

				JReadAll(fromFD, value);
				if (!value->IsEmpty() &&
					value->GetLastCharacter() == '\n')
					{
					const JSize len = value->GetLength();
					value->RemoveSubstring(len, len);
					}
				}
			}
		else
			{
			value->Clear();
			}

		return kJTrue;
		}
	else
		{
		return JSubstitute::Evaluate(s, startIndex, matchRange, value);
		}
}

/******************************************************************************
 GetExecRange (static)

 *****************************************************************************/

JBoolean
CBMacroSubstitute::GetExecRange
	(
	const JString&	s,
	const JIndex	startIndex,
	JIndexRange*	matchRange
	)
{
	const JSize len = s.GetLength();
	JIndex endIndex = startIndex+1;
	JSize count     = 1;
	while (endIndex <= len)
		{
		const JCharacter c = s.GetCharacter(endIndex);
		if (c == '\\')
			{
			endIndex++;
			}
		else if (c == '$' &&
				 endIndex < len && s.GetCharacter(endIndex+1) == '(')
			{
			count++;
			}
		else if (c == ')')
			{
			count--;
			if (count == 0)
				{
				break;
				}
			}
		endIndex++;
		}

	matchRange->Set(startIndex, JMin(endIndex, len));
	return JI2B( count == 0 && matchRange->GetLength() > 2 );
}

/******************************************************************************
 TurnOnEscapes (private)

 *****************************************************************************/

void
CBMacroSubstitute::TurnOnEscapes()
{
	SetEscape('n', "\n");
	SetEscape('t', "\t");
	SetEscape('b', "\b");
	SetEscape('f', &kJForwardDeleteKey, 1);
	SetEscape('l', &kJLeftArrow,        1);
	SetEscape('r', &kJRightArrow,       1);
	SetEscape('u', &kJUpArrow,          1);
	SetEscape('d', &kJDownArrow,        1);
}

/******************************************************************************
 TurnOffEscapes (private)

 *****************************************************************************/

void
CBMacroSubstitute::TurnOffEscapes()
{
	ClearEscape('n');
	ClearEscape('t');
	ClearEscape('b');
	ClearEscape('f');
	ClearEscape('l');
	ClearEscape('r');
	ClearEscape('u');
	ClearEscape('d');
}
