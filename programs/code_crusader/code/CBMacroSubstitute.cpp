/******************************************************************************
 CBMacroSubstitute.cpp

	BASE CLASS = JSubstitute

	Copyright © 1998 by John Lindal.

 *****************************************************************************/

#include "CBMacroSubstitute.h"
#include "cbmUtil.h"
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <JStringIterator.h>
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

bool
CBMacroSubstitute::Evaluate
	(
	JStringIterator&	iter,
	JString*			value
	)
	const
{
	JUtf8Character c;
	if (iter.Next(&c, kJIteratorStay) && c == '(')
		{
		iter.SkipNext();
		iter.BeginMatch();
		if (CBMBalanceForward(kCBCLang, &iter, &c))
			{
			iter.SkipPrev();
			*value = iter.FinishMatch().GetString();
			iter.SkipNext();

			auto* me = const_cast<CBMacroSubstitute*>(this);
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
					msg.Prepend(JGetString("Error::CBGlobal"));
					JGetUserNotification()->ReportError(msg);
					}

				JReadAll(fromFD, value);
				value->TrimWhitespace();
				}
			}
		else
			{
			value->Clear();
			}

		return true;
		}
	else
		{
		return JSubstitute::Evaluate(iter, value);
		}
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

	JUtf8Byte c[] = { '\0', '\0' };

	c[0] = kJForwardDeleteKey;
	SetEscape('f', c);

	c[0] = kJLeftArrow;
	SetEscape('l', c);

	c[0] = kJRightArrow;
	SetEscape('r', c);

	c[0] = kJUpArrow;
	SetEscape('u', c);

	c[0] = kJDownArrow;
	SetEscape('d', c);
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
