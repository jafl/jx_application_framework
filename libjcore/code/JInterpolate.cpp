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

	BASE CLASS = public JSubstitute

	Copyright © 1998 by Dustin Laurence.  All rights reserved.
	Copyright © 2005 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JInterpolate.h>
#include <JRegex.h>
#include <jAssert.h>

static const JCharacter* theVariablePattern = "[+-]?[0-9]+|\\{[A-Za-z0-9_]+\\}";

/******************************************************************************
 Constructor

 *****************************************************************************/

JInterpolate::JInterpolate()
	:
	JSubstitute(),
	itsSource(NULL),
#ifdef PCRE_MAJOR
	itsRegex(NULL),
#endif
	itsMatchList(NULL)
{
	DefineVariables(theVariablePattern);
}

JInterpolate::JInterpolate
	(
	const JCharacter*			source,
#ifdef PCRE_MAJOR
	const pcre*					regex,
#endif
	const JArray<JIndexRange>*	matchList
	)
	:
	JSubstitute(),
	itsSource(source),
#ifdef PCRE_MAJOR
	itsRegex(regex),
#endif
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
#ifdef PCRE_MAJOR
		itsRegex == NULL  ||
#endif
		itsMatchList == NULL)
		{
		return kJTrue;	// assume ContainsError() was called
		}

	assert( !name.IsEmpty() );

	const JSize matchCount = itsMatchList->GetElementCount();

	JInteger matchNumber;
#ifdef PCRE_MAJOR
	if (name.GetFirstCharacter() == '{')
		{
		assert( name.GetLastCharacter() == '}' );

		const JString s = name.GetSubstring(2, name.GetLength()-1);
		matchNumber     = pcre_get_stringnumber(itsRegex, s);
		if (matchNumber <= 0)
			{
			return kJTrue;
			}
		}
	else
#endif
		{
		const JBoolean wasNumber = name.ConvertToInteger(&matchNumber);
		assert( wasNumber );

		if (matchNumber < 0) // Wrap so negatives count from the end
			{
			matchNumber += matchCount;
			}
		}

	const JIndex matchIndex = matchNumber + 1;	// first subexpression is index 2

	if (1 <= matchIndex && matchIndex <= matchCount)
		{
		const JIndexRange thisRange = itsMatchList->GetElement(matchIndex);
		value->Set(itsSource + thisRange.first - 1, thisRange.GetLength() );
		}
	else
		{
		value->Clear();
		}

	// We accept *any* match.
	return kJTrue;
}
