/******************************************************************************
 JRegex.cpp

	JRegex is the JCore regular-expression pattern-matching class.  It provides
	regular expression-based search and replace facilities with a convenient,
	safe interface which is idiomatic in both C++ and JCore.
	
	JRegex uses a JIndexRange-oriented interface.  This means that JRegex can
	do in-place search and replace on very large strings without copying more
	text than is necessary (and in fact was originally designed for use in a
	text editor whose buffers are single strings and can contain megabytes).

	JCore Regex Extensions

	JRegex normally provides some simple extensions to the regular
	expression language provided by regex; see SetNoJExtended and
	JSubstitute for details.  However, normal C-style backslash escapes for
	special characters like '\n' are still not expanded, because in many
	context (notably C++ source) this is done by some other tool.  If you
	need to do this yourself it is probably because you are allowing your
	users to type in a regular expression string, in which case you
	probably need fine control over which escapes can be used ('\0' or '\b'
	are often inappropriate, for example), which would be clumsy to supply
	in JRegex.  Instead JRegex can be queried for the objects it uses to
	interpolate matches and to expand excapes so they can be customized as
	necessary.

	USAGE:

	In spite of the number of methods defined, usage is straightforward.  A
	pattern is set with the constructor or the SetPattern method.  Matches are
	then performed with Match... and friends, while replacement is performed
	with the Replace method.  See the notes on the individual functions for
	details.  The apparent complexity of the class derives from the large number
	of convenience functions in the Match... family.  Most of them are written
	in terms of a small number of fundamental operations, and could easily have
	been written by the client.  In other words, while JRegex has a large
	"surface area" (Brad Cox's term for a class with a large public interface),
	once a small number are understood the usage of the others will be obvious.
	The number of methods that must be understood to use JRegex can be as small
	as two for simple problems.

	The best way to understand JRegex is to start by using it with only two
	methods, SetPattern and Match(JCharacter*).  Then add Match(JCharacter*,
	JIndexRange*), and then Match(JCharacter*, JArray<JIndexRange>*).  At
	this point the entire match interface should be readily understood.  After
	adding SetReplacePattern and Replace to your reportoire, the rest of the
	interface is just customization, information, and extra Match functions you
	can learn as needed.

	Unfortunately the test suite is quite involved and not really a good way
	to learn by example.  Example code needs to be supplied soon.

	DESIGN:

	JRegex uses a dollar sign '$' for match interpolations.  Backslashes
	are used in some tools, but for several reasons it is better for both
	user and implementor to use a different symbol for character escapes
	and for match interpolation.  One is that dollar signs avoid the
	'backslashitis' which results in source code because C already expands
	them (of course, you still have to deal with this problem for character
	escapes).  They also correspond to the preferred usage in Perl,
	arguably the premier regex tool.

	The Match functions assert that a pattern has successfully been set.
	It might be more forgiving to simply return an error condition instead.
	This would, however considerably complicate the Match interface, and I
	don't think it's worth it.  It is infeasible to simply have a default
	pattern because there is no clear choice and this would only make
	things more confusing.  Just check to see that it set before calling a
	Match... function.

	Replace could assert or return an error value if called before a
	successful call to SetReplacePattern rather than assuming a default
	value of ""; again, this would crud up the code.  At some point, the
	complexity inherent in error checking is as error prone as not having
	the error checking, and I feel it is important to keep the basic JRegex
	interface clean, so the error return solution is not feasible.  The
	assert solution is a feasible alternative, and can easily be adopted if
	there is an outcry.  However, the empty string is an obvious choice, so
	the current system should be as clear as having JString's default
	constructor set its value to "".  Underuse of defaults can be almost as
	bad as overuse.

	IMPLEMENTATION:

	JRegex is really a (somewhat complex) layer over Henry Spencer's (new
	DFA, not old NFA) regex package.  It provides an improved, native C++
	interface, but the real work is underneath, in regex.  Thanks, Henry.

	JRegex was written with the intention of making it fully NULL-safe
	sometime in the future.  Some things are safe now, but the
	function-level documentation mostly refers to whether the function
	*interface* is NULL-safe; the internal implementations may still not
	be.  The actual behaviors with NULLs embedded in patterns, strings,
	replacement patterns, and as replacement metacharacters are almost
	completely untested so far, as nobody has actually needed this
	capability.  Anywhere the notes say a something is NULL-safe this
	should be read as "this is *rumored* to be NULL-safe."  Sorry about the
	vagueness, but as no one has actually needed this yet it hasn't been
	worth implementing, and you're welcome to regard it as entirely
	NULL-unsafe for now.

	The private RegExec function asserts that it did not run out of memory
	or get passed an invalid argument.  I think JRegex can guarantee that
	regex is never fed an invalid argument, so this is probably fine.

	Finally, there is one restriction inherent in the underlying regex
	package: backreferences are supported with basic but not extended
	expressions (yet).

	JRegex version: 1.1.5

	regex version: alpha 3.6

	BASE CLASS = <NONE>

	Copyright © 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JRegex.h>
#include <JSubstitute.h>
#include <JInterpolate.h>
#include <jAssert.h>

#ifdef PCRE_MAJOR

struct regmatch_t
{
	int rm_so;	/* start of match */
	int rm_eo;	/* end of match */
};

#endif

	// Constants
	const JCharacter* JRegex::kError = "Error::JRegex";

	// Constant static data (i.e. ordinary file scope constants)
#ifdef PCRE_MAJOR
	static const int defaultCFlags = PCRE_MULTILINE;
	static const int defaultEFlags = 0;
#else
	// WARNING: JRegex assumes that REG_PEND and REG_STARTEND are *always*
	//          set, so they must always be in the defaults list!
	static const int defaultCFlags = REG_NEWLINE | REG_PEND | REG_EXTENDED;
	static const int defaultEFlags = REG_STARTEND;
#endif

	// JAFL 5/11/98
	const JString JRegex::theSpecialCharList = ".[]\\?*+{}|()^$";

/******************************************************************************
 Constructor

	The form which takes a pattern argument automatically does a SetPattern,
	which is convenient.  However, it also asserts that this pattern was
	successfully compiled, which can be rather inconvenient if you make a
	mistake.  So don't make mistakes (or don't use that constructor). :-)

 *****************************************************************************/

JRegex::JRegex()
	:
	itsPattern(),
	itsNULLCount(0),
#ifdef PCRE_MAJOR
	itsRegex(NULL),
#else
//	itsRegex(), // It's Spencer's struct, we don't know the fields
#endif
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags),
	itsReplacePattern(NULL),
	itsInterpolator(NULL),
	itsEscapeEngine(NULL),
	itsState(kEmpty),
	itsNoJExtendedFlag(kJFalse),
	itsLiteralReplaceFlag(kJFalse),
	itsMatchCaseFlag(kJFalse)
{
	Allocate();
}


JRegex::JRegex
	(
	const JCharacter* pattern,
	const JBoolean    useJExtended // = kJFalse
	)
	:
	itsPattern(),
	itsNULLCount(0),
#ifdef PCRE_MAJOR
	itsRegex(NULL),
#else
//	itsRegex(), // It's Spencer's struct, we don't know the fields
#endif
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags),
	itsReplacePattern(NULL),
	itsInterpolator(NULL),
	itsEscapeEngine(NULL),
	itsState(kEmpty),
	itsNoJExtendedFlag(useJExtended),
	itsLiteralReplaceFlag(kJFalse),
	itsMatchCaseFlag(kJFalse)
{
	Allocate();
	SetPatternOrDie(pattern); // Nothing else to do in a constructor
}


JRegex::JRegex
	(
	const JCharacter* pattern,
	const JSize       length,
	const JBoolean    useJExtended // = kJFalse
	)
	:
	itsPattern(),
	itsNULLCount(0),
#ifdef PCRE_MAJOR
	itsRegex(NULL),
#else
//	itsRegex(), // It's Spencer's struct, we don't know the fields
#endif
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags),
	itsReplacePattern(NULL),
	itsInterpolator(NULL),
	itsEscapeEngine(NULL),
	itsState(kEmpty),
	itsNoJExtendedFlag(useJExtended),
	itsLiteralReplaceFlag(kJFalse),
	itsMatchCaseFlag(kJFalse)
{
	Allocate();
	SetPatternOrDie(pattern, length); // Nothing else to do in a constructor
}


// Copy constructor
JRegex::JRegex
	(
	const JRegex& source
	)
	:
	itsPattern(),
	itsNULLCount( static_cast<JSize>(-1) ), // Garbage, will be recalculated by SetPattern
#ifdef PCRE_MAJOR
	itsRegex(NULL),
#else
//	itsRegex(), // It's Spencer's struct, we don't know the fields
#endif
	itsCFlags(source.itsCFlags),
	itsEFlags(source.itsEFlags),
	itsReplacePattern(NULL),
	itsInterpolator(NULL),
	itsEscapeEngine(NULL),
	itsState(kEmpty),
	itsNoJExtendedFlag(source.itsNoJExtendedFlag),
	itsLiteralReplaceFlag(source.itsLiteralReplaceFlag),
	itsMatchCaseFlag(source.itsMatchCaseFlag)
{
	Allocate();

	CopyPatternRegex(source);
	assert(itsNULLCount == source.itsNULLCount); // Paranoid consistency check :-)
}


void
JRegex::Allocate()
{
	itsReplacePattern = new JString;
	assert(itsReplacePattern != NULL);

	// itsInterpolator is not created until needed
#ifndef PCRE_MAJOR
	itsEscapeEngine = new JSubstitute;
	assert(itsEscapeEngine != NULL);
	itsEscapeEngine->SetPureEscapeEngine();
	itsEscapeEngine->IgnoreUnrecognized();
	itsEscapeEngine->SetRegexExtensions();
#endif

	#ifdef	JRE_ALLOC_CHECK
	numRegexAlloc = 0;
	#endif
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JRegex::~JRegex()
{
	RegFree();

	delete itsReplacePattern;
	itsReplacePattern = NULL;

	delete itsInterpolator;
	delete itsEscapeEngine;
}

/******************************************************************************
 assignment operator

 *****************************************************************************/

const JRegex&
JRegex::operator=
	(
	const JRegex& source
	)
{
	if (&source != this)
		{
		itsCFlags = source.itsCFlags;
		itsEFlags = source.itsEFlags;
		CopyPatternRegex(source);
		}
	return *this;
}

/******************************************************************************
 BackslashForLiteral (static)

	Insert backslashes so the string will be interpreted literally by the
	regex parser.

 *****************************************************************************/

JString
JRegex::BackslashForLiteral
	(
	const JCharacter* text
	)
{
	JString s = text;
	for (JIndex i=s.GetLength(); i>=1; i--)
		{
		if (NeedsBackslashToBeLiteral(s.GetCharacter(i)))
			{
			s.InsertSubstring("\\", i);
			}
		}

	return s;
}

/******************************************************************************
 SetPattern

	Sets 'pattern' as the regular expression for subsequent matches.
	Returns kJFalse if the pattern could not be compiled, kJTrue otherwise.
	If the compile fails the pattern remains set until the next call to
	SetPattern(); it can be examined with GetPattern().

	The versions which take a JString or a pointer and a length allow
	compilation of patterns which contain NULL (and also allow the
	programmer to get the length wrong and write a pattern which matches
	random garbage or seg faults, so be careful) or are not NULL
	terminated.  The pointer only form obviously allows neither pathology.

 *****************************************************************************/

JError
JRegex::SetPattern
	(
	const JCharacter* pattern
	)
{
	return SetPattern(pattern, strlen(pattern));
}


JError
JRegex::SetPattern
	(
	const JCharacter* pattern,
	const JSize       length
	)
{
	if (JStringCompare(itsPattern, itsPattern.GetLength(),
					   pattern, length) != 0)
		{
		CopyPattern(pattern, length);
		return RegComp();
		}
	else if (itsState != kReady)
		{
		return RegComp();
		}
	else
		{
		return JNoError();
		}
}


JError
JRegex::SetPattern
	(
	const JString& pattern
	)
{
	return SetPattern(pattern, pattern.GetLength());
}

/******************************************************************************
 SetPatternOrDie

	All forms are like the corresponding SetPattern forms, but they assert that
	the set succeeds.  A minor convenience, since a set followed by an assert
	seems to happen pretty frequently.

 *****************************************************************************/

void
JRegex::SetPatternOrDie
	(
	const JCharacter* pattern
	)
{
	const JError error = SetPattern(pattern);
	assert_ok( error );
}


void
JRegex::SetPatternOrDie
	(
	const JCharacter* pattern,
	const JSize       length
	)
{
	const JError error = SetPattern(pattern, length);
	assert_ok( error );
}


void
JRegex::SetPatternOrDie
	(
	const JString& pattern
	)
{
	const JError error = SetPattern(pattern);
	assert_ok( error );
}

/******************************************************************************
 SetNoJExtended

	The NoJExtended flag turns off the JCore extensions to the regular
	expression language.  It defaults to false, so the JCore extensions
	(actually they or variants are quite common in regular expression
	tools) are active.  If set to true, the following constructs (actually
	defined in JSubstitute.cc) will not work:

		"\d"   a digit, [0-9]
		"\D"   a non-digit
		"\w"   a word character, [a-zA-Z0-9_]
		"\W"   a non-word character
		"\s"   a whitespace character, [ \f\n\r\t\v]
		"\S"   a non-whitespace character
		"\<"   an anchor just before a word (between \w and \W)
		"\>"   an anchor just after a word (between \W and \w)

	For more information, see the JSubstitute documentation (which is where
	these shorthands are actually implemented).

	Note that these are implemented by the escape engine and are set on
	every compile, so that changing their values directly through the
	engine rather than with this function has no effect.  At the moment,
	they are all or nothing.

 *****************************************************************************/

#ifndef PCRE_MAJOR

void
JRegex::SetNoJExtended
	(
	const JBoolean yesNo // = kJTrue
	)
{
	if (itsNoJExtendedFlag != yesNo)
		{
		itsNoJExtendedFlag = yesNo;

		if (itsState == kReady)
			{
			itsState = kRecompile;
			}
		}
}

#endif

/******************************************************************************
 GetSubCount

	Returns the number of parenthesized subexpressions in the compiled expression.
	Returns zero if there is no compiled expression, if the expression has no
	subexpressions, or if the expression was compiled with SetMatchOnly().

 *****************************************************************************/

JSize
JRegex::GetSubCount() const
{
#ifdef PCRE_MAJOR

	if (itsRegex == NULL)
		{
		return 0;
		}
	else
		{
		int count;
		const int result = pcre_fullinfo(itsRegex, NULL, PCRE_INFO_CAPTURECOUNT, &count);
		assert( result == 0 );
		return count;
		}

#else

	if (IsMatchOnly() || itsState == kEmpty || itsState == kCannotCompile)
		{
		return 0;
		}
	else
		{
		// Recompile if we see the impossible flag value set by SetCompileOption
		if (itsRegex.re_nsub == static_cast<size_t>(-1) )
			{
			JRegex* mutate = const_cast<JRegex*>(this);
			mutate->CompileOrDie();
			}

		return itsRegex.re_nsub;
		}

#endif
}

/******************************************************************************
 Match and friends

	The Match family performs various kinds of searches on strings specified
	in various ways, and return various kinds of information.  A successful
	SetPattern() is required before their use.  There are a large number of
	them for convenience, but they can be understood systematically:

	There are four ways to specify the string to be searched.  The first three
	ways use a standard null-terminated string.  The Match(...) functions
	search the entire string, while the MatchFrom(...) functions search from
	a given index to the end of the string and the
	MatchAfter(...) functions search starting after a given subrange.  Finally,
	the MatchWithin(...) functions search only within a given subrange--they
	also allow nulls to be embedded in the string and treat them as ordinary
	characters, since there is no need for a null terminator to determine where
	the Match should stop.  The performance difference between these cases is
	negligible.

	Note that the MatchFrom and MatchAfter functions do not check that their
	index argument is actually within the string because this would be too
	costly on a very large string, while the MatchWithin functions cannot because
	they treat NULL as an ordinary character.  This makes it easy to specify a
	search which goes outside the intended string and gives surprising results
	or seg faults.  Be careful with these types of searches.

	There are three basic kinds of searches: report on the first match, report
	on the last match, and an iterated search for all matches.  Since in
	reality all searches are start to finish, the latter two are implemented
	internally in terms of the first (and therefore obviously are more costly).

	There are three basic types of information which can be returned about
	each match.  The simplest is whether it occured, or a count of matches
	if for an iterated search.  The second is the range(s) which the pattern
	matched, or (leave the parameter unchanged if no match was found), and the
	third is to also report the range(s) where each parenthesized subexpression
	matched.  Obtaining match ranges is expensive in terms of performance,
	obtaining subexpression matches is moreso.

 *****************************************************************************/

/******************************************************************************

	The following functions simply return kJTrue if there is at least one match
	in a string, which can be specified in various ways.  They may all be
	called even when MatchOnly has been specified.

 *****************************************************************************/

/******************************************************************************
 Match

 *****************************************************************************/

JBoolean
JRegex::Match
	(
	const JCharacter* str
	)
	const
{
	JIndexRange r;
	return RegExec(str, 0, strlen(str), &r, NULL);
}

/******************************************************************************
 MatchFrom

 *****************************************************************************/

JBoolean
JRegex::MatchFrom
	(
	const JCharacter* str,
	const JIndex      index
	)
	const
{
	return MatchWithin(str, JIndexRange(index, strlen(str) ) );
}

/******************************************************************************
 MatchAfter

 *****************************************************************************/

JBoolean
JRegex::MatchAfter
	(
	const JCharacter*  str,
	const JIndexRange& range
	)
	const
{
	return MatchFrom(str, range.last+1);
}

/******************************************************************************
 MatchWithin

 *****************************************************************************/

JBoolean
JRegex::MatchWithin
	(
	const JCharacter*	str,
	const JIndexRange&	range
	)
	const
{
	JIndexRange r;
	return RegExec(str, range.first-1, range.last, &r, NULL);
}

/******************************************************************************

 	Iterator versions

 *****************************************************************************/

/******************************************************************************
 MatchAll

	Perhaps inobviously, may NOT be called if SetMatchOnly is true (because
	internally it must obtain the region matched so it knows where to start
	looking for the next).

 *****************************************************************************/

JSize
JRegex::MatchAll
	(
	const JCharacter* str
	)
	const
{
	JIndexRange match(1,0);
	JSize matchCount = 0;

	while (MatchAfter(str, match, &match))
		{
		if ( match.IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if (str[match.first-1] == '\0')
				{
				break; // Avoid calling MatchAfter with match beyond end of string
				}
			else
				{
				match += 1;
				}
			}
		matchCount++;
		}
	return matchCount;
}

/******************************************************************************

	The following functions return the substring range which contains the
	first match in the specified string.  Naturally, they may not be called
	when MatchOnly is set.

 *****************************************************************************/

/******************************************************************************
 Match

 *****************************************************************************/

JBoolean
JRegex::Match
	(
	const JCharacter* str,
	JIndexRange*      match
	)
	const
{
	assert( match != NULL );
	assert( !IsMatchOnly() );

	return RegExec(str, 0, strlen(str), match, NULL);
}

/******************************************************************************
 MatchFrom

 *****************************************************************************/

JBoolean
JRegex::MatchFrom
	(
	const JCharacter* str,
	const JIndex      index,
	JIndexRange*      match
	)
	const
{
	return MatchWithin(str, JIndexRange(index, strlen(str) ), match);
}

/******************************************************************************
 MatchAfter

	Both ranges may be the same object, very useful in while loops.

 *****************************************************************************/

JBoolean
JRegex::MatchAfter
	(
	const JCharacter*  str,
	const JIndexRange& range,
	JIndexRange*       match
	)
	const
{
	return MatchFrom(str, range.last+1, match);
}

/******************************************************************************
 MatchWithin

 *****************************************************************************/

JBoolean
JRegex::MatchWithin
	(
	const JCharacter*  str,
	const JIndexRange& range,
	JIndexRange*       match
	)
	const
{
	assert( match != NULL );
	assert( !IsMatchOnly() );

	return RegExec(str, range.first-1, range.last, match, NULL);
}

/******************************************************************************

	The following functions find the substring range which contains the last
	match in the specified string and return the number of matches in the
	string.

 *****************************************************************************/

/******************************************************************************
 MatchLast

 *****************************************************************************/

JSize
JRegex::MatchLast
	(
	const JCharacter* str,
	JIndexRange*      match
	)
	const
{
	assert(match != NULL);

	JIndexRange thisMatch(1, 0);
	JSize matchCount = 0;

	while (MatchAfter(str, thisMatch, &thisMatch))
		{
		if ( thisMatch.IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if (str[thisMatch.first-1] == '\0')
				{
				break; // Avoid calling MatchAfter with match beyond end of string
				}
			else
				{
				thisMatch += 1;
				}
			}
		matchCount++;
		}
	if (matchCount > 0)
		{
		*match = thisMatch;
		}

	return matchCount;
}

/******************************************************************************
 MatchLastWithin

 *****************************************************************************/

JSize
JRegex::MatchLastWithin
	(
	const JCharacter*  str,
	const JIndexRange& range,
	JIndexRange*       match
	)
	const
{
	assert(match != NULL);
	assert( !IsMatchOnly() );

	JIndexRange searchRegion = range;
	JSize matchCount = 0;

	while ( MatchWithin(str, searchRegion, match) )
		{
		if ( match->IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if ( match->first > range.first )
				{
				break; // Avoid calling MatchWithin with match beyond end of string
				}
			else
				{
				*match += 1;
				}
			}
		matchCount++;
		searchRegion.first = match->last + 1;
		}

	return matchCount;
}

/******************************************************************************

 	Iterator versions

 *****************************************************************************/

/******************************************************************************
 MatchAll

	As above, but stores all the matches in matchList, which may never be NULL.

 *****************************************************************************/

JSize
JRegex::MatchAll
	(
	const JCharacter*    str,
	JArray<JIndexRange>* matchList
	)
	const
{
	assert(matchList != NULL);

	matchList->RemoveAll();

	JIndexRange match(1, 0);
	JSize matchCount = 0;

	while (MatchAfter(str, match, &match))
		{
		matchList->AppendElement(match);
		if ( match.IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if (str[match.first-1] == '\0')
				{
				break; // Avoid calling MatchAfter with match beyond end of string
				}
			else
				{
				match += 1;
				}
			}
		matchCount++;
		}
	return matchCount;
}

/******************************************************************************
 MatchAllWithin

 *****************************************************************************/

JSize
JRegex::MatchAllWithin
	(
	const JCharacter*    str,
	const JIndexRange&   range,
	JArray<JIndexRange>* matchList
	)
	const
{
	assert(matchList != NULL);
	assert( !IsMatchOnly() );

	JIndexRange match, searchRegion = range;
	JSize matchCount = 0;

	matchList->RemoveAll();

	while ( MatchWithin(str, searchRegion, &match) )
		{
		matchList->AppendElement(match);
		searchRegion.first = match.last + 1;
		if ( match.IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if ( match.first > range.first )
				{
				break; // Avoid calling MatchWithin with match beyond end of string
				}
			else
				{
				match += 1;
				}
			}
		matchCount++;
		}

	return matchCount;
}

/******************************************************************************

	The following functions return a list of the ranges which matched the overall
	expression and all subexpressions in the specified string.

 *****************************************************************************/

/******************************************************************************
 Match

	As the previous, except that subMatch is a JArray which will be set to
	a GetSubCount() length list of what each subexpression matched.
	GetFirstElement() is set to the overall match, and the other elements
	are set to the subexpressions matched.  SubMatchList may never be NULL.

 *****************************************************************************/

JBoolean
JRegex::Match
	(
	const JCharacter*    str,
	JArray<JIndexRange>* subMatchList
	)
	const
{
	assert( subMatchList != NULL );
	assert( !IsMatchOnly() );

	JIndexRange r;
	return RegExec(str, 0, strlen(str), &r, subMatchList);
}

/******************************************************************************
 MatchFrom

 *****************************************************************************/

JBoolean
JRegex::MatchFrom
	(
	const JCharacter*    str,
	const JIndex         index,
	JArray<JIndexRange>* subMatchList
	)
	const
{
	return MatchWithin(str, JIndexRange(index, strlen(str) ), subMatchList);
}

/******************************************************************************
 MatchAfter

 *****************************************************************************/

JBoolean
JRegex::MatchAfter
	(
	const JCharacter*    str,
	const JIndexRange&   range,
	JArray<JIndexRange>* subMatchList
	)
	const
{
	return MatchFrom(str, range.last+1, subMatchList);
}

/******************************************************************************
 MatchWithin

 *****************************************************************************/

JBoolean
JRegex::MatchWithin
	(
	const JCharacter*    str,
	const JIndexRange&   range,
	JArray<JIndexRange>* subMatchList
	)
	const
{
	assert( subMatchList != NULL );
	assert( !IsMatchOnly() );

	JIndexRange r;
	return RegExec(str, range.first-1, range.last, &r, subMatchList);
}

/******************************************************************************
 MatchLastWithin

 *****************************************************************************/

JSize
JRegex::MatchLastWithin
	(
	const JCharacter*    str,
	const JIndexRange&   range,
	JArray<JIndexRange>* subMatchList
	)
	const
{
	assert(subMatchList != NULL);
	assert( !IsMatchOnly() );

	JIndexRange searchRegion = range;
	JSize matchCount = 0;
	JIndexRange match;

	while ( MatchWithin(str, searchRegion, subMatchList) )
		{
		match = subMatchList->GetFirstElement();
		if ( match.IsEmpty() ) // Avoid infinite loop if get a null match!
			{
			if ( match.first > range.first )
				{
				break; // Avoid calling MatchWithin with match beyond end of string
				}
			else
				{
				match += 1;
				}
			}
		matchCount++;
		searchRegion.first = match.last + 1;
		}

	if (matchCount > 0 && match.IsEmpty())
		{
		subMatchList->AppendElement(match);
		}

	return matchCount;
}

/******************************************************************************
 MatchBackward

	MatchLast can be used to effectively search backwards, but by itself it is
	inefficient on large buffers because searches will start from the beginning.
	The MatchBackward functions attempt to provide a better algorithm for
	backwards searching on large buffers by a trial-and-error scheme.

	The first form is identical to MatchLast but (hopefully) faster.  The
	index is not included in the search range; in other words, the search
	is over the range (1, index-1).  Index may be any number from 1 to
	the length of the buffer plus 1 (but if it is 1 obviously all tests
	must fail). The caller must ensure that it is not too long.

	The second form is intended for repeated interactive searches.  The
	MatchBackward algorithm will often find several previous matches, and
	it is wasteful to throw them away and only report the last one.  The
	list is not guaranteed to have more than one element (zero if there is
	no match before the given point), but whatever it contains is
	guaranteed to be the previous matches in order *front to back*.  In
	other words, if it has three elements, the third element is the first
	one encountered going backwards from the given point, the second
	element is the second one encountered, and the first element is the
	third.  The return value is the number found.

	* The second form is untested because no one has really needed it yet.

 *****************************************************************************/

JBoolean
JRegex::MatchBackward
	(
	const JCharacter* str,
	const JIndex      index,
	JIndexRange*      match
	)
	const
{
	assert( !IsMatchOnly() );
	assert(match != NULL);
	assert(index > 0);

	// Need to find good values for these guys
	JSize decrement = 1000;
	const JSize multiplier = 10;

	JIndexRange searchRange;
	JInteger from = index;
	do
		{
		if (from == 1)
			{
			return kJFalse; // We failed, and *match is unchanged
			}
		from = from - decrement;
		if (from < 1)
			{
			from = 1;
			}
		decrement *= multiplier;
		searchRange.Set(from, index-1);
		}
		while ( !MatchLastWithin(str, searchRange, match) );

	return kJTrue; // We succeeded, and *match contains the match
}

JSize
JRegex::MatchBackward
	(
	const JCharacter*    str,
	const JIndex         index,
	JArray<JIndexRange>* matchList
	)
	const
{
	assert( !IsMatchOnly() );
	assert(matchList != NULL);
	assert(index > 0);

	// Need to find good values for these guys
	JSize decrement = 1000;
	const JSize multiplier = 10;

	JIndexRange searchRange;
	JSize numFound = 0;
	JInteger from = index;
	do
		{
		if (from == 1)
			{
			return 0; // We failed and *matchList is empty
			}
		from = from - decrement;
		if (from < 1)
			{
			from = 1;
			}
		decrement *= multiplier;
		searchRange.Set(from, index-1);
		numFound = MatchAllWithin(str, searchRange, matchList);
		}
		while (numFound == 0);

	return numFound; // We succeeded, and *matchList contains what we found
}

#ifdef PCRE_MAJOR

/******************************************************************************
 GetSubexpressionIndex

 *****************************************************************************/

JBoolean
JRegex::GetSubexpressionIndex
	(
	const JCharacter*	name,
	JIndex*				index
	)
	const
{
	if (itsRegex != NULL)
		{
		const int i = pcre_get_stringnumber(itsRegex, name);
		if (i > 0)
			{
			*index = i+1;	// first subexpression is index 2
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 GetSubexpressionIndex

 *****************************************************************************/

JBoolean
JRegex::GetSubexpression
	(
	const JCharacter*			str,
	const JCharacter*			name,
	const JArray<JIndexRange>&	matchList,
	JString*					s
	)
	const
{
	JIndex i;
	if (GetSubexpressionIndex(name, &i))
		{
		const JIndexRange r = matchList.GetElement(i);
		if (!r.IsNothing())
			{
			s->Set(str + r.first-1, r.GetLength());
			return kJTrue;
			}
		}

	s->Clear();
	return kJFalse;
}

#endif

/******************************************************************************
 GetMatchInterpolator

 *****************************************************************************/

JInterpolate*
JRegex::GetMatchInterpolator()
	const
{
	if (itsInterpolator == NULL)
		{
		// This function is conceptually const

		JRegex* mutableThis = const_cast<JRegex*>(this);
		mutableThis->itsInterpolator = new JInterpolate;
		assert(itsInterpolator != NULL);
		}

	return itsInterpolator;
}

/******************************************************************************
 InterpolateMatches

	Given a list of substring ranges (matchList) and the string the ranges
	refer to (sourceString), InterpolateMatches builds and returns the
	actual replacement string the internal replace pattern, stored with a
	previous call to SetReplacePattern (default is "").  Presumably
	matchList is the list of subexpression matches from a search on
	sourceString, but this is not required.

	InterpolateMatches uses (a derived class of) JSubstitute to do the
	interpolation, and each JRegex has its own interpolation object which
	can be obtained with GetMatchInterpolator() to adjust how escape codes
	and variable patterns are recognized.  The default behavior for $
	patterns should rarely if ever need modification, but changing the
	escape codes can be very useful.  By default C escapes are not expanded
	since this is most convenient for patterns specified in source code; in
	user-specified patterns in interactive programs it may be better to add
	these escapes so that non-printing characters may be entered
	conveniently (the same is true for the search pattern escapes).

	The default $ pattern behavior is as follows.  A $ followed by a
	positive number N is replaced by the (N+1)st element of matchList
	(pedantically, the substring of sourceString referred to by the first
	element of matchList), counting from the beginning of the list.  A $
	followed by a negative number -N is replaced by the Nth element of
	matchList, *counting from the end of the list*.  If the number refers
	to an element which does not exist in matchList, the replacement string
	is "".  Replacement is done in one pass, so matches which contain '$N'
	are not subject to further replacement.  Maniacs who want a convenient
	way to, say, iterate until a fixed point is reached should contact the
	author.

	${x} is replaced by the subexpression named "x".

	The above rules are easier to understand in the normal case where
	matchList was generated by a prevous match.  Then $0 is the entire
	match, while $1 is the first subexpression, $2 the second, and so forth
	up to $9, numbering subexpressions by counting left parentheses from
	left to right.  Similarly, $-1 is the last subexpression, $-2 is the
	second to the last, and so on.  If the pattern actually only contained
	four subexpressions, then $5 through $9 and $-6 through $-10 would be
	replaced by "", while both $0 and $-5 would be replaced by the overall
	match.  Similarly, both $1 and $-4 would be replaced by the first
	parenthesized subexpression, $2 and $-3) by the second, $3 and $-2 by
	the third, and finally $4 and $-1 by the fourth.

 *****************************************************************************/

JString
JRegex::InterpolateMatches
	(
	const JCharacter*          sourceString,
	const JArray<JIndexRange>& matchList
	)
	const
{
	assert(sourceString != NULL);

	JString replaceString = *itsReplacePattern;

	JInterpolate* interpolator = GetMatchInterpolator();
#ifdef PCRE_MAJOR
	interpolator->SetMatchResults(sourceString, itsRegex, &matchList);
#else
	interpolator->SetMatchResults(sourceString, &matchList);
#endif
	interpolator->Substitute(&replaceString);
	interpolator->SetMatchResults(NULL, NULL, NULL);

	if (itsMatchCaseFlag && !matchList.IsEmpty())
		{
		replaceString.MatchCase(sourceString, matchList.GetElement(1));
		}

	return replaceString;
}

/******************************************************************************
 Replace

	Replace() interpolates the regex's replacement pattern into string,
	using the results of a previous search on that same string.  This
	functionality could be duplicated by the client, but it would be much
	less convenient.  The JRegex property LiteralReplace controls whether
	match interpolation takes place.

	The three argument form which takes only ranges always substitutes the
	replacement pattern without first using InterpolateMatches, regardless
	of the value of ReplaceLiteral, and so only requires a simple range
	rather than a match list.  However, it is a deprecated function which
	will eventually go away.

 *****************************************************************************/

void
JRegex::Replace
	(
	JString*                   str,
	const JArray<JIndexRange>& matchList,
	JIndexRange*               newRange
	)
	const
{
	if (itsLiteralReplaceFlag)
		{
		Replace(str,
		        matchList.GetElement(1),
		        newRange);
		}
	else
		{
		assert( str != NULL );
		assert( newRange != NULL );

		str->ReplaceSubstring(matchList.GetElement(1),
		                      InterpolateMatches(*str, matchList),
		                      newRange);
		}
}


void
JRegex::Replace
	(
	JString*           str,
	const JIndexRange& oldRange,
	JIndexRange*       newRange
	)
	const
{
	assert( str != NULL );
	assert( newRange != NULL );

	str->ReplaceSubstring(oldRange,
	                      *itsReplacePattern,
	                      newRange);
}

/******************************************************************************
 SetReplacePattern

	Sets the replacement pattern.  The return value is true if the
	replacement pattern is 'clean', that is if all $ signs are escaped or
	followed by a number (the exact definition is that used by the
	underlying JSubstitute match object), but this is only a convenience
	for interactive programs checking their user's input; the pattern is
	set whether it is clean or not.  IsCleanPattern() can be used to perform
	the same test without changing the pattern.

	The syntax of the replacement source is under the underlying
	JInterpolate object's control, but a summary of the default behavior is
	as follows.  The replacement metacharacter is '$', which marks the
	beginning of a replacement token.  'Unclean' replacement patterns are
	patterns which contain a metacharacter not immediately followed either
	by a second metacharacter or by an optional sign ('-' or '+') preceding
	one or more decimal digits.  All other patterns are 'clean'.  Clean
	patterns will be replaced by their corresponding match value, while
	unclean patterns are left intact except their '$' is removed; to insert
	a literal '$' preface it with a backslash.

	In other words, the only clean replacement tokens are those of the form
	'$[+-]?[0-9]+', and a replacement pattern is clean unless it contains
	at least one unclean replacement token.

	See InterpolateMatches for how the replacement pattern will be used.

	The default replace pattern is "", that is all matches are replaced by
	the empty string.  This simplifies the replace interface because there is
	always a valid replace string, but arguably makes it easier to screw up by
	forgetting to set the replace string.  If enough people complain that the
	benefits are not worth the drawbacks, this situation can change.

	As usual, the forms that take a JString or a JCharacter* and a length are
	NULL-safe.

	If an error is found, and errRange is not NULL, it is set to the
	offending range of characters.

 *****************************************************************************/

JError
JRegex::SetReplacePattern
	(
	const JString& pattern,
	JIndexRange*   errRange
	)
{
	*itsReplacePattern = pattern;

	JIndexRange r;
	if (errRange == NULL)
		{
		errRange = &r;
		}
	return (GetMatchInterpolator())->ContainsError(*itsReplacePattern, errRange);
}

/******************************************************************************
 RestoreDefaults

	Restores the default values of both the compile-time and run-time
	options and the replacement pattern.

 *****************************************************************************/

JError
JRegex::RestoreDefaults()
{
	itsEFlags = defaultEFlags;

	if (itsCFlags != defaultCFlags)
		{
		itsCFlags = defaultCFlags;

		if (itsState == kReady)
			{
			itsState = kRecompile;
			}
		}

	itsReplacePattern->Clear();
	return RegComp();
}

/******************************************************************************
 CopyPatternRegex (private)

	Takes care of transfering the pattern, regex, replace pattern, and
	itsCompiledFlag states (but not the options or ranges) from one JRegex to
	another.

 *****************************************************************************/

void
JRegex::CopyPatternRegex
	(
	const JRegex& source
	)
{
	if (source.itsState == kEmpty)
		{
		RegFree();
		}
	else if (source.itsState == kCannotCompile)
		{
		RegFree();
		CopyPattern(source);
		itsState = kCannotCompile;
		}
	else
		{
		// Should never fail if source has already compiled!
		SetPatternOrDie(source.itsPattern, source.itsPattern.GetLength() );
		}

	SetReplacePattern(*source.itsReplacePattern);
//	JBoolean set = SetReplacePattern(*source.itsReplacePattern);
//	assert( set );
}

/******************************************************************************
 CopyPattern (private)

	Copies 'pattern' into the pattern buffer.

	WARNING: It is possible for humans to become trapped in the pattern
	buffer at the whim of the Paramount scriptwriters; when this happens,
	rescue is temporarily impossible, and so only brief, half-hearted
	attempts should be made.  Rescue becomes possible only after further
	plot complications, generally at the last possible instant.

 *****************************************************************************/

void
JRegex::CopyPattern
	(
	const JCharacter* pattern,
	const JSize       length
	)
{
	itsPattern.Set(pattern, length);

	itsNULLCount = 0;
	for (JIndex i=0; i<length; i++)
		{
		if (pattern[i] == '\0')
			{
			itsNULLCount++;
			}
		}
}

void
JRegex::CopyPattern
	(
	const JRegex& source
	)
{
	itsPattern   = source.itsPattern;
	itsNULLCount = source.itsNULLCount;
}

/******************************************************************************
 SetCompileOption (private)

 *****************************************************************************/

void
JRegex::SetCompileOption
	(
	const int      option,
	const JBoolean setClear
	)
{
	int oldCFlags = itsCFlags;

	RawSetOption(&itsCFlags, option, setClear);

	if (itsState == kReady && oldCFlags != itsCFlags)
		{
		itsState = kRecompile;

#ifndef PCRE_MAJOR
		if ( (oldCFlags&REG_NOSUB) && !(itsCFlags&REG_NOSUB) )
			{
			// Set ridiculous value as flag for GetSubCount() to recompile;
			// for compatibility with future regex versions we never trust
			// an nsub calculated by compiling with REG_NOSUB
			itsRegex.re_nsub = static_cast<size_t>(-1);
			}
#endif
		}
}

/******************************************************************************
 SetExecuteOption (private)

 *****************************************************************************/

void
JRegex::SetExecuteOption
	(
	const int      option,
	const JBoolean setClear
	)
{
	RawSetOption(&itsEFlags, option, setClear);
}

/******************************************************************************
 RawSetOption (private)

 *****************************************************************************/

void
JRegex::RawSetOption
	(
	int*           flags,
	const int      option,
	const JBoolean setClear
	)
{
	if (setClear)
		{
		*flags |= option;
		}
	else
		{
		*flags &= ~option;
		}
}

/******************************************************************************
 CompileOrDie (private)

 *****************************************************************************/

void
JRegex::CompileOrDie()
{
	const JError error = RegComp();
	assert_ok( error );
}

/******************************************************************************
 RegComp (private)

 *****************************************************************************/

JError
JRegex::RegComp()
{
	RegFree();
	itsState = kCannotCompile;

	if (itsPattern.IsEmpty())
		{
		return JRegexError(kError, "empty pattern", 0);
		}

#ifdef PCRE_MAJOR
	assert( itsRegex == NULL );

	const char* errorMessage;
	int errorOffset;
	itsRegex = pcre_compile(itsPattern, itsCFlags,
							&errorMessage, &errorOffset, NULL);
	const int retVal = (itsRegex == NULL ? 1 : 0);
#else
	assert( RawGetOption(itsCFlags, REG_PEND) ); // Paranoia

	if (itsNoJExtendedFlag)
		{
		itsEscapeEngine->ClearRegexExtensions();
		}
	else
		{
		itsEscapeEngine->SetRegexExtensions();
		}

	JString pattern = itsPattern;
	itsEscapeEngine->Substitute(&pattern);

	itsRegex.re_endp = pattern.GetCString() + pattern.GetLength();
	const int retVal = regcomp(&itsRegex, pattern, itsCFlags);
#endif

	if (retVal == 0)
		{
		#ifdef JRE_ALLOC_CHECK
		++numRegexAlloc;
		assert(numRegexAlloc == 1);
		#endif

		// I assume that nothing needs be freed unless the compile succeeds
		itsState = kReady;

		return JNoError();
		}
	else
		{
#ifdef PCRE_MAJOR
		const JRegexError error(kError, errorMessage, errorOffset+1);
#else
		const JSize msgLength = regerror(retVal, &itsRegex, NULL, 0);
		JCharacter* msg       = new JCharacter[ msgLength+1 ];
		assert( msg != NULL );

		regerror(retVal, &itsRegex, msg, msgLength);
		msg[ msgLength ] = '\0';

		const JRegexError error(kError, msg, 0);
		delete[] msg;
#endif

		#ifdef JRE_PRINT_COMPILE_ERRORS
		cout << "Compile error: " << error.GetMessage() << endl;
		#endif

		#ifdef JRE_ALLOC_CHECK
		assert(numRegexAlloc == 0);
		#endif

		return error;
		}
}

/******************************************************************************
 RegExec (private)

 *****************************************************************************/

inline JIndexRange
jMakeIndexRange
	(
	const regmatch_t& regmatch		// {-1,-1} => nothing
	)
{
	return JIndexRange(regmatch.rm_so+1, regmatch.rm_eo > 0 ? regmatch.rm_eo : 0);
};

JBoolean
JRegex::RegExec
	(
	const JCharacter*		str,
	const JSize				offset,
	const JSize				length,
	JIndexRange*			matchRange,
	JArray<JIndexRange>*	matchList		// can be NULL
	)
	const
{
	assert( str != NULL );
	assert( itsState != kEmpty && itsState != kCannotCompile );

	#ifdef JRE_ALLOC_CHECK
	assert( numRegexAlloc == 1 );
	#endif

	if (itsState == kRecompile)
		{
		JRegex* mutate = const_cast<JRegex*>(this);
		mutate->CompileOrDie();
		}

	const JSize subCount = GetSubCount();
	regmatch_t* pmatch   = NULL;
	JSize nmatch         = 1;

#ifdef PCRE_MAJOR
	nmatch = (subCount+1)*3;
#else
	if (matchList != NULL)
		{
		nmatch = subCount+1;
		}
#endif

	pmatch = new regmatch_t[ nmatch ];
	assert( pmatch != NULL );

#ifdef PCRE_MAJOR
	int returnCode = pcre_exec(itsRegex, NULL, str, length, offset,
							   itsEFlags, (int*) pmatch, nmatch);
	if (returnCode > 0)
		{
		nmatch     = returnCode;
		returnCode = 0;
		}
#else
	assert( RawGetOption(itsEFlags, REG_STARTEND) ); // Paranoia

	pmatch[0].rm_so = offset;
	pmatch[0].rm_eo = length;

	int eFlags = itsEFlags;
	if (!IsSingleLine() && offset > 0 && str[offset-1] != '\n')
		{
		eFlags |= REG_NOTBOL;
		}
	if (!IsSingleLine() &&
		str[ offset + length ] != '\n' &&
		str[ offset + length ] != '\0')
		{
		eFlags |= REG_NOTEOL;
		}

	const int returnCode = regexec(&itsRegex, str, nmatch, pmatch, eFlags);
#endif

	if (returnCode == 0)
		{
		*matchRange = jMakeIndexRange(pmatch[0]);

		if (matchList != NULL)
			{
			matchList->RemoveAll();
			JIndex i;
			for (i=0; i<nmatch; i++)
				{
				matchList->AppendElement(jMakeIndexRange(pmatch[i]));
				}
			JIndexRange empty;
			for (i=nmatch; i<=subCount; i++)
				{
				matchList->AppendElement(empty);
				}
			}

		return kJTrue;
		}
	else if (returnCode == REG_NOMATCH)
		{
/*		MatchLastWithin() requires that values are not changed!

		matchRange->SetToNothing();

		if (matchList != NULL)
			{
			matchList->RemoveAll();
			}
*/
		return kJFalse;
		}
	else
		{
		assert( 0 );	// unexpected error
		return kJFalse;
		}
}

/******************************************************************************
 RegFree (private)

 *****************************************************************************/

void
JRegex::RegFree()
{
#ifdef PCRE_MAJOR
	if (itsRegex != NULL)
		{
		pcre_free(itsRegex);
		itsRegex = NULL;

		#ifdef JRE_ALLOC_CHECK
		numRegexAlloc--;
		#endif
		}
#else
	if (itsState != kEmpty && itsState != kCannotCompile)
		{
		::regfree(&itsRegex);
		itsState = kCannotCompile;

		#ifdef JRE_ALLOC_CHECK
		numRegexAlloc--;
		#endif
		}
#endif

	#ifdef JRE_ALLOC_CHECK
	assert(numRegexAlloc == 0);
	#endif
}
