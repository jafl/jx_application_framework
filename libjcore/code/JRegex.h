/******************************************************************************
 JRegex.h

	Interface for the JRegex class.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JRegex
#define _H_JRegex

#include "JStringMatch.h"
#include "JPtrArray-JString.h"
#include "JError.h"
#include <pcre.h>

class JStringMatch;
class JInterpolate;

class JRegex
{
	friend class JStringIterator;
	friend class JSubstitute;

public:

	JRegex();
	JRegex(const JString& pattern);
	JRegex(const JUtf8Byte* pattern);

	virtual ~JRegex();

	JRegex(const JRegex& source);
	const JRegex& operator=(const JRegex& source);

// Setting and examining patterns

	const JString&	GetPattern() const;

	JError	SetPattern(const JString& pattern);
	JError	SetPattern(const JUtf8Byte* pattern);

	void	SetPatternOrDie(const JString& pattern);
	void	SetPatternOrDie(const JUtf8Byte* pattern);

	static JBoolean	NeedsBackslashToBeLiteral(const JUtf8Byte c);
	static JBoolean	NeedsBackslashToBeLiteral(const JUtf8Character& c);
	static JString	BackslashForLiteral(const JString& text);

// Pattern-related settings and statistics

	JSize    	GetSubexpressionCount() const;
	JBoolean	GetSubexpressionIndex(const JUtf8Byte* name, JIndex* index) const;
	JBoolean	GetSubexpressionIndex(const JString& name, JIndex* index) const;

// Matching

	JBoolean	Match(const JString& str) const;
	void		Split(const JString& str, JPtrArray<JString>* strPartList,
					  const JBoolean includeSeparators = kJFalse) const;

// Setting and testing options

	JBoolean IsCaseSensitive() const;
	void     SetCaseSensitive(const JBoolean yesNo = kJTrue);

	JBoolean IsSingleLine() const;
	void     SetSingleLine(const JBoolean yesNo = kJTrue);

	JBoolean IsLineBegin() const;
	void     SetLineBegin(const JBoolean yesNo = kJTrue);

	JBoolean IsLineEnd() const;
	void     SetLineEnd(const JBoolean yesNo = kJTrue);

	JBoolean IsMatchCase() const;
	void     SetMatchCase(const JBoolean yesNo = kJTrue);

	JError RestoreDefaults();

protected:

	JStringMatch	MatchForward(const JString& str, const JIndex byteIndex) const;
	JStringMatch	MatchBackward(const JString& str, const JIndex byteIndex) const;

private:

	enum PatternState
	{
		kEmpty,         // Do not have a pattern, itsRegex is freed
		kCannotCompile, // Have one, but it can't be compiled, itsRegex is freed
		kRecompile,     // A parameter changed, recompile before next match, itsRegex is allocated
		kReady          // and rarin' to go, obviously itsRegex is allocated
	};

	JString			itsPattern;
	PatternState	itsState;
	pcre*			itsRegex;
	int				itsCFlags;
	int				itsEFlags;

	int	numRegexAlloc;		// only used if JRE_ALLOC_CHECK is defined

	// Static data

	static const JString	theSpecialCharList;

private:

	void CopyPatternRegex(const JRegex& source);

	JStringMatch	MatchLastWithin(const JString& str, const JUtf8ByteRange& range) const;

	void SetCompileOption(const int option, const JBoolean setClear);
	void SetExecuteOption(const int option, const JBoolean setClear);
	void RawSetOption(int* flags, const int option, const JBoolean setClear);

	JBoolean RawGetOption(const int flags, const int option) const;

	// The basic regex library functions, translated

	JError			Compile();
	JStringMatch	Match(const JString& str,
						  const JSize byteOffset, const JSize byteCount,
						  const JBoolean includeSubmatches) const;
	void			CleanUp();

public:

	static const JUtf8Byte* kError;

	class JRegexError : public JError
	{
		friend class JRegex;

	protected:

		JRegexError(const JUtf8Byte* type, const JUtf8Byte* message,
					const JIndex index)
			:
			JError(type, message),
			itsIndex(index)
		{ };

	public:

		JBoolean
		GetErrorIndex
			(
			JIndex* index
			)
			const
		{
			*index = itsIndex;
			return JI2B(itsIndex > 0);
		};

	private:

		JIndex	itsIndex;
	};
};


/******************************************************************************
 NeedsBackslashToBeLiteral (static)

	JAFL 5/11/98

	Returns kJTrue if the given character needs to be backslashed
	in order to be treated as a literal by the regex compiler.

 *****************************************************************************/

inline JBoolean
JRegex::NeedsBackslashToBeLiteral
	(
	const JUtf8Byte c
	)
{
	return theSpecialCharList.Contains(&c, 1);
}

inline JBoolean
JRegex::NeedsBackslashToBeLiteral
	(
	const JUtf8Character& c
	)
{
	return theSpecialCharList.Contains(c);
}

/******************************************************************************
 GetPattern

 *****************************************************************************/

inline const JString&
JRegex::GetPattern() const
{
	return itsPattern;
}

/******************************************************************************
 CaseSensitive

	Controls whether matches will be case sensitive (the default) or case
	insensitive.

	Performance note: changing this option can cause a recompile before the
	next match.

 *****************************************************************************/

inline void
JRegex::SetCaseSensitive
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetCompileOption(PCRE_CASELESS, !yesNo);
}

inline JBoolean
JRegex::IsCaseSensitive() const
{
	return !RawGetOption(itsCFlags, PCRE_CASELESS);
}

/******************************************************************************
 SingleLine

	Controls whether the entire string is considered a single line or whether
	newlines are considered to indicate line boundaries (the default).  This
	option is independent of SetLineBegin() and SetLineEnd().

	Performance note: changing this option can cause a recompile before the
	next match.

 *****************************************************************************/

inline void
JRegex::SetSingleLine
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetCompileOption(PCRE_MULTILINE, !yesNo);
	SetCompileOption(PCRE_DOTALL, yesNo);
}

inline JBoolean
JRegex::IsSingleLine() const
{
	return !RawGetOption(itsCFlags, PCRE_MULTILINE);
}

/******************************************************************************
 LineBegin

	Controls whether the beginning of the string to be matched is considered
	to begin a line for purposes of matching '^'.  Default is for '^' to match
	at the beginning of the string.  This option is independent of
	SetSingleLine().

 *****************************************************************************/

inline void
JRegex::SetLineBegin
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetExecuteOption(PCRE_NOTBOL, !yesNo);
}

inline JBoolean
JRegex::IsLineBegin() const
{
	return !RawGetOption(itsEFlags, PCRE_NOTBOL);
}

/******************************************************************************
 LineEnd

	Controls whether the end of the string to be matched is considered to end a
	line for purposes of matching '$'.  Default is for '$' to match at the end
	of the string.  This option is independent of SetSingleLine().

 *****************************************************************************/

inline void
JRegex::SetLineEnd
	(
	const JBoolean yesNo // = kJTrue
	)
{
	SetExecuteOption(PCRE_NOTEOL, !yesNo);
}

inline JBoolean
JRegex::IsLineEnd() const
{
	return !RawGetOption(itsEFlags, PCRE_NOTEOL);
}

/******************************************************************************
 MatchForward (protected)

	If a match is not found, the returned JStringMatch will be empty.

 *****************************************************************************/

inline JStringMatch
JRegex::MatchForward
	(
	const JString&	str,
	const JIndex	byteIndex
	)
	const
{
	return Match(str, byteIndex-1, str.GetByteCount(), kJTrue);
}

/******************************************************************************
 GetSubexpressionIndex

 *****************************************************************************/

inline JBoolean
JRegex::GetSubexpressionIndex
	(
	const JString& name,
	JIndex*        index
	)
	const
{
	return GetSubexpressionIndex(name.GetBytes(), index);
}

#endif
