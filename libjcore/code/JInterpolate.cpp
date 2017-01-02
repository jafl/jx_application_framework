/******************************************************************************
 JInterpolate.cpp

	JInterpolate does the interpolation of replacement strings for JRegex.

	A summary of the default behavior is as follows.  The replacement
	metacharacter is '$', which marks the beginning of a replacement token.
	'Unclean' replacement patterns are patterns which contain a
	metacharacter not immediately followed either by a second metacharacter
	or by an optional sign ('-' or '+') preceding one or more decimal
	digits.  All other patterns are 'clean'.  Clean patterns will be
	replaced by their corresponding match value, while unclean patterns are
	left intact except their '$' is removed; to insert a literal '$'
	preface it with a backslash.

	In other words, the only clean replacement tokens are those of the form
	'$[+-]?[0-9]+' and '${...}', and a replacement pattern is clean unless
	it contains at least one unclean replacement token.

	The $ pattern behavior is as follows.  A $ followed by a positive
	number N is replaced by the (N+1)st element of matchList, counting from
	the beginning of the list.  A $ followed by a negative number -N is
	replaced by the Nth element of matchList, *counting from the end of the
	list*.  If the number refers to an element which does not exist in
	matchList, the replacement string is "".  Replacement is done in one
	pass, so matches which contain '$N' are not subject to further
	replacement.  Maniacs who want a convenient way to, say, iterate until
	a fixed point is reached should write their own loop.

	${x} is replaced by the subexpression named "x".

	The above rules are easier to understand in the normal case where
	matchList was generated by a previous match.  Then $0 is the entire
	match, while $1 is the first subexpression, $2 the second, and so forth
	up to $9, numbering subexpressions by counting left parentheses from
	left to right.  Similarly, $-1 is the last subexpression, $-2 is the
	second to the last, and so on.  If the pattern actually only contained
	four subexpressions, then $5 through $9 and $-6 through $-10 would be
	replaced by "", while both $0 and $-5 would be replaced by the overall
	match.  Similarly, both $1 and $-4 would be replaced by the first
	parenthesized subexpression, $2 and $-3) by the second, $3 and $-2 by
	the third, and finally $4 and $-1 by the fourth.

	BASE CLASS = public JSubstitute

	Copyright (C) 1998 by Dustin Laurence.  All rights reserved.
	Copyright (C) 2005 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JInterpolate.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

static const JUtf8Byte* theVariablePattern = "[+-]?[0-9]+|\\{[A-Za-z0-9_]+\\}";

/******************************************************************************
 Constructor

 *****************************************************************************/

JInterpolate::JInterpolate()
	:
	JSubstitute(),
	itsSource(NULL),
	itsRegex(NULL),
	itsMatchList(NULL)
{
	DefineVariables(theVariablePattern);
}

JInterpolate::JInterpolate
	(
	const JUtf8Byte*			source,
	const JRegex*				regex,
	const JArray<JIndexRange>*	matchList
	)
	:
	JSubstitute(),
	itsSource(source),
	itsRegex(regex),
	itsMatchList(matchList)
{
	DefineVariables(theVariablePattern);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JInterpolate::~JInterpolate()
{
}

/******************************************************************************
 GetValue (virtual protected)

 *****************************************************************************/

JBoolean
JInterpolate::GetValue
	(
	const JString&	name,
	JString*		value
	)
	const
{
	if (itsSource == NULL ||
		itsRegex == NULL  ||
		itsMatchList == NULL)
		{
		return kJFalse;
		}

	assert( !name.IsEmpty() );
	value->Clear();

	JIndex matchNumber;
	if (name.GetFirstCharacter() == '{')
		{
		assert( name.GetLastCharacter() == '}' );

		JString s = name;
		JStringIterator iter(&s);
		iter.RemoveNext();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		iter.RemovePrev();

		if (!itsRegex->GetSubexpressionIndex(s, &matchNumber))
			{
			return kJFalse;
			}
		}
	else
		{
		JInteger n;
		const JBoolean wasNumber = name.ConvertToInteger(&n);
		assert( wasNumber );

		if (n < 0)	// Wrap so negatives count from the end
			{
			n += itsMatchList->GetElementCount();
			}
		matchNumber = n;
		}

	const JIndex matchIndex = matchNumber + 1;	// first subexpression is index 2

	if (itsMatchList->IndexValid(matchIndex))
		{
		const JIndexRange thisRange = itsMatchList->GetElement(matchIndex);
		value->Set(itsSource + thisRange.first - 1, thisRange.GetCount() );
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
