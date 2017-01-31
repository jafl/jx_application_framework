/******************************************************************************
 JRegex.cpp

	JRegex provides regular expression-based search with a convenient, safe
	interface which is idiomatic in both C++ and JCore.

	USAGE:

	A pattern is set with the constructor or the SetPattern method.
	Matches are then performed with Match*().

	Search & replace must be done using a JStringIterator.

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
	things more confusing.  Just check to see that it is set before calling
	a Match... function.

	IMPLEMENTATION:

	JRegex is a thin layer over the PCRE package.  It provides an improved,
	native C++ interface, but the real work is underneath, in PCRE.

	The private Match function asserts that it did not run out of memory
	or get passed an invalid argument.  I think JRegex can guarantee that
	regex is never fed an invalid argument, so this is probably fine.

	BASE CLASS = none

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	Copyright (C) 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JRegex.h>
#include <JStringIterator.h>
#include <JMinMax.h>
#include <pcreposix.h>	// for regmatch_t
#include <jAssert.h>

// Debugging:
//#define JRE_ALLOC_CHECK
//#define JRE_PRINT_COMPILE_ERRORS

// Constants
const JUtf8Byte* JRegex::kError = "Error::JRegex";

// Constant static data (i.e. ordinary file scope constants)
static const int defaultCFlags = PCRE_MULTILINE | PCRE_UTF8;
static const int defaultEFlags = 0;

// JAFL 5/11/98
const JString JRegex::theSpecialCharList(".[]\\?*+{}|()^$", 0, kJFalse);

/******************************************************************************
 Constructor

	The form which takes a pattern argument automatically does a SetPattern,
	which is convenient.  However, it also asserts that this pattern was
	successfully compiled, which can be rather inconvenient if you make a
	mistake.  So don't make mistakes (or don't use that constructor). :-)

 *****************************************************************************/

JRegex::JRegex()
	:
	itsPattern(kJFalse),
	itsState(kEmpty),
	itsRegex(NULL),
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags)
{
}


JRegex::JRegex
	(
	const JString& pattern
	)
	:
	itsPattern(kJFalse),
	itsState(kEmpty),
	itsRegex(NULL),
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags)
{
	SetPatternOrDie(pattern); // Nothing else to do in a constructor
}


JRegex::JRegex
	(
	const JUtf8Byte* pattern
	)
	:
	itsPattern(kJFalse),
	itsState(kEmpty),
	itsRegex(NULL),
	itsCFlags(defaultCFlags),
	itsEFlags(defaultEFlags)
{
	SetPatternOrDie(pattern); // Nothing else to do in a constructor
}

/******************************************************************************
 Copy constructor

 *****************************************************************************/

JRegex::JRegex
	(
	const JRegex& source
	)
	:
	itsState(kEmpty),
	itsRegex(NULL),
	itsCFlags(source.itsCFlags),
	itsEFlags(source.itsEFlags)
{
	CopyPatternRegex(source);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JRegex::~JRegex()
{
	CleanUp();
}

/******************************************************************************
 Assignment operator

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
 CopyPatternRegex (private)

	Takes care of transfering the pattern, regex, and itsCompiledFlag
	states (but not the options or ranges) from one JRegex to another.

 *****************************************************************************/

void
JRegex::CopyPatternRegex
	(
	const JRegex& source
	)
{
	if (source.itsState == kEmpty)
		{
		CleanUp();
		}
	else if (source.itsState == kCannotCompile)
		{
		CleanUp();
		itsPattern = source.itsPattern;
		itsState   = kCannotCompile;
		}
	else if (source.itsState == kReady)
		{
		SetPatternOrDie(source.itsPattern);
		}
	else	// kRecompile - not sure if it will work
		{
		SetPattern(source.itsPattern);
		}
}

/******************************************************************************
 BackslashForLiteral (static)

	Insert backslashes so the string will be interpreted literally by the
	regex parser.

 *****************************************************************************/

JString
JRegex::BackslashForLiteral
	(
	const JString& text
	)
{
	const JUtf8Character backslash = "\\";

	JString s = text;
	JStringIterator iter(&s, kJIteratorStartAtEnd);
	JUtf8Character c;
	while (iter.Prev(&c))
		{
		if (NeedsBackslashToBeLiteral(c))
			{
			iter.Insert(backslash);
			}
		else if (c == '\0')
			{
			iter.RemoveNext();
			iter.Insert("\\x0");
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

 *****************************************************************************/

JError
JRegex::SetPattern
	(
	const JString& pattern
	)
{
	if (itsPattern != pattern)	// may contain NULL, so cannot call other version
		{
		itsPattern.Set(pattern);
		return Compile();
		}
	else if (itsState != kReady)
		{
		return Compile();
		}
	else
		{
		return JNoError();
		}
}

JError
JRegex::SetPattern
	(
	const JUtf8Byte* pattern
	)
{
	if (itsPattern != pattern)
		{
		itsPattern.Set(pattern);
		return Compile();
		}
	else if (itsState != kReady)
		{
		return Compile();
		}
	else
		{
		return JNoError();
		}
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
	const JString& pattern
	)
{
	const JError error = SetPattern(pattern);
	assert_ok( error );
}

void
JRegex::SetPatternOrDie
	(
	const JUtf8Byte* pattern
	)
{
	const JError error = SetPattern(pattern);
	assert_ok( error );
}

/******************************************************************************
 GetSubexpressionCount

	Returns the number of parenthesized subexpressions in the compiled
	expression.  Returns zero if there is no compiled expression or if the
	expression has no subexpressions.

 *****************************************************************************/

JSize
JRegex::GetSubexpressionCount()
	const
{
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
}

/******************************************************************************
 Match

	Returns kJTrue if our pattern matches the given string.

 *****************************************************************************/

JBoolean
JRegex::Match
	(
	const JString& str
	)
	const
{
	JUtf8ByteRange r;
	return JNegate( Match(str, 0, str.GetByteCount(), kJFalse).IsEmpty() );
}

/******************************************************************************
 MatchBackward (protected)

	The MatchBackward function attempts to provide an efficient algorithm
	for backwards searching on large buffers by a trial-and-error scheme.

	The byteIndex is included in the search range; in other words, the
	search is over the range [1, byteIndex].  Index may be any number from
	1 to the length of the buffer.

 *****************************************************************************/

JStringMatch
JRegex::MatchBackward
	(
	const JString&	str,
	const JIndex	byteIndex
	)
	const
{
	assert( byteIndex > 0 );

	// Adjustable parameters
	JInteger decrement     = 1000;
	const JSize multiplier = 5;

	JUtf8ByteRange searchRange;
	JInteger from = byteIndex;
	do
		{
		from = JMax(1L, from - decrement);
		searchRange.Set(from, byteIndex);

		const JStringMatch m = MatchLastWithin(str, searchRange);
		if (!m.IsEmpty())
			{
			return m;
			}

		decrement *= multiplier;	// for next iteration
		}
		while (from > 1);

	return JStringMatch(str, JUtf8ByteRange(), this);
}

/******************************************************************************
 MatchLastWithin (private)

 *****************************************************************************/

JStringMatch
JRegex::MatchLastWithin
	(
	const JString&			str,
	const JUtf8ByteRange&	range
	)
	const
{
	JUtf8ByteRange searchRegion = range;

	JStringMatch result(str, JUtf8ByteRange());
	while (1)
		{
		const JStringMatch m = Match(str, searchRegion.first-1, range.GetCount(), kJTrue);
		if (m.GetUtf8ByteRange().IsNothing())
			{
			break;
			}
		else if (m.IsEmpty()) // Avoid infinite loop if get a null match!
			{
			searchRegion.first = m.GetUtf8ByteRange().first + 1;
			}
		else
			{
			result             = m;
			searchRegion.first = m.GetUtf8ByteRange().last + 1;
			}
		}

	return result;
}

/******************************************************************************
 GetSubexpressionIndex

 *****************************************************************************/

JBoolean
JRegex::GetSubexpressionIndex
	(
	const JUtf8Byte*	name,
	JIndex*				index
	)
	const
{
	if (itsRegex != NULL)
		{
		const int i = pcre_get_stringnumber(itsRegex, name);
		if (i > 0)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 RestoreDefaults

	Restores the default values of both the compile-time and run-time options.

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

	return Compile();
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

inline void
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
 Compile (private)

 *****************************************************************************/

JError
JRegex::Compile()
{
	CleanUp();
	itsState = kCannotCompile;

	if (itsPattern.IsEmpty())
		{
		return JRegexError(kError, "empty pattern", 0);
		}

	assert( itsRegex == NULL );

	const char* errorMessage;
	int errorOffset;
	itsRegex = pcre_compile(itsPattern.GetBytes(), itsCFlags,
							&errorMessage, &errorOffset, NULL);
	const int retVal = (itsRegex == NULL ? 1 : 0);

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
		const JRegexError error(kError, errorMessage, errorOffset+1);

		#ifdef JRE_PRINT_COMPILE_ERRORS
		std::cout << "Compile error: " << error.GetMessage() << std::endl;
		#endif

		#ifdef JRE_ALLOC_CHECK
		assert(numRegexAlloc == 0);
		#endif

		return error;
		}
}

/******************************************************************************
 Match (private)

 *****************************************************************************/

inline JUtf8ByteRange
jMakeRange
	(
	const regmatch_t& regmatch		// {-1,-1} => nothing
	)
{
	return JUtf8ByteRange(regmatch.rm_so+1, regmatch.rm_eo > 0 ? regmatch.rm_eo : 0);
};

JStringMatch
JRegex::Match
	(
	const JString&	str,
	const JSize		byteOffset,
	const JSize		byteCount,
	const JBoolean	includeSubmatches
	)
	const
{
	assert( itsState != kEmpty && itsState != kCannotCompile );

	#ifdef JRE_ALLOC_CHECK
	assert( numRegexAlloc == 1 );
	#endif

	if (itsState == kRecompile)
		{
		const JError error = const_cast<JRegex*>(this)->Compile();
		assert_ok( error );
		}

	const JSize subCount = GetSubexpressionCount();
	regmatch_t* pmatch   = NULL;
	int nmatch           = (subCount+1)*3;

	pmatch = jnew regmatch_t[ nmatch ];
	assert( pmatch != NULL );

	nmatch = pcre_exec(itsRegex, NULL, str.GetBytes(), byteCount, byteOffset,
					   itsEFlags, (int*) pmatch, nmatch);
	if (nmatch > 0)
		{
		const JUtf8ByteRange m0 = jMakeRange(pmatch[0]);

		JArray<JUtf8ByteRange>* list = NULL;
		if (includeSubmatches)
			{
			list = jnew JArray<JUtf8ByteRange>;
			assert( list != NULL );

			for (JIndex i=1; i<nmatch; i++)
				{
				list->AppendElement(jMakeRange(pmatch[i]));
				}
			}

		jdelete [] pmatch;
		return JStringMatch(str, m0, this, list);
		}
	else if (nmatch != PCRE_ERROR_NOMATCH &&
			 nmatch != PCRE_ERROR_RECURSIONLIMIT &&
			 nmatch != PCRE_ERROR_BADOFFSET)
		{
		std::cerr << "unexpected error from PCRE: " << nmatch << std::endl;
		}

	jdelete [] pmatch;
	return JStringMatch(str, JUtf8ByteRange(), this);
}

/******************************************************************************
 CleanUp (private)

 *****************************************************************************/

void
JRegex::CleanUp()
{
	if (itsRegex != NULL)
		{
		pcre_free(itsRegex);
		itsRegex = NULL;

		#ifdef JRE_ALLOC_CHECK
		numRegexAlloc--;
		#endif
		}

	#ifdef JRE_ALLOC_CHECK
	assert(numRegexAlloc == 0);
	#endif
}
