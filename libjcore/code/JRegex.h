/******************************************************************************
 JRegex.h

	Interface for the JRegex class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JRegex
#define _H_JRegex

#include "jx-af/jcore/JStringMatch.h"
#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JError.h"
#include <pcre.h>

class JStringMatch;

class JRegex
{
	friend class JStringIterator;
	friend class JSubstitute;

public:

	enum IncludeSubmatches
	{
		kIgnoreSubmatches  = 0,
		kIncludeSubmatches = 1
	};

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

	static bool		NeedsBackslashToBeLiteral(const JUtf8Byte c);
	static bool		NeedsBackslashToBeLiteral(const JUtf8Character& c);
	static JString	BackslashForLiteral(const JString& text);

// Pattern-related settings and statistics

	JSize	GetSubexpressionCount() const;
	bool	GetSubexpressionIndex(const JUtf8Byte* name, JIndex* index) const;
	bool	GetSubexpressionIndex(const JString& name, JIndex* index) const;

// Matching

	bool			Match(const JString& str) const;
	JStringMatch	Match(const JString& str, const IncludeSubmatches includeSubmatches) const;

// Setting and testing options

	bool IsCaseSensitive() const;
	void SetCaseSensitive(const bool yesNo = true);

	bool IsSingleLine() const;
	void SetSingleLine(const bool yesNo = true);

	bool IsLineBegin() const;
	void SetLineBegin(const bool yesNo = true);

	bool IsLineEnd() const;
	void SetLineEnd(const bool yesNo = true);

	bool IsUtf8() const;
	void SetUtf8(const bool yesNo = true);

	JError	RestoreDefaults();

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

	void SetCompileOption(const int option, const bool setClear);
	void SetExecuteOption(const int option, const bool setClear);
	void RawSetOption(int* flags, const int option, const bool setClear);

	bool RawGetOption(const int flags, const int option) const;

	// The basic regex library functions, translated

	JError			Compile();
	JStringMatch	Match(const JString& str,
						  const JSize byteOffset, const JSize byteCount,
						  const IncludeSubmatches includeSubmatches) const;
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

		bool
		GetErrorIndex
			(
			JIndex* index
			)
			const
		{
			*index = itsIndex;
			return itsIndex > 0;
		};

	private:

		JIndex	itsIndex;
	};
};


/******************************************************************************
 NeedsBackslashToBeLiteral (static)

	JAFL 5/11/98

	Returns true if the given character needs to be backslashed
	in order to be treated as a literal by the regex compiler.

 *****************************************************************************/

inline bool
JRegex::NeedsBackslashToBeLiteral
	(
	const JUtf8Byte c
	)
{
	return theSpecialCharList.Contains(&c, 1);
}

inline bool
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
	const bool yesNo // = true
	)
{
	SetCompileOption(PCRE_CASELESS, !yesNo);
}

inline bool
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
	const bool yesNo // = true
	)
{
	SetCompileOption(PCRE_MULTILINE, !yesNo);
	SetCompileOption(PCRE_DOTALL, yesNo);
}

inline bool
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
	const bool yesNo // = true
	)
{
	SetExecuteOption(PCRE_NOTBOL, !yesNo);
}

inline bool
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
	const bool yesNo // = true
	)
{
	SetExecuteOption(PCRE_NOTEOL, !yesNo);
}

inline bool
JRegex::IsLineEnd() const
{
	return !RawGetOption(itsEFlags, PCRE_NOTEOL);
}

/******************************************************************************
 Utf8

	Controls whether the string is treated as utf-8 or raw bytes.

 *****************************************************************************/

inline void
JRegex::SetUtf8
	(
	const bool yesNo // = true
	)
{
	SetCompileOption(PCRE_UTF8, !yesNo);
}

inline bool
JRegex::IsUtf8() const
{
	return RawGetOption(itsCFlags, PCRE_UTF8);
}

/******************************************************************************
 Match

	Returns true if our pattern matches the given string.

 *****************************************************************************/

inline bool
JRegex::Match
	(
	const JString& str
	)
	const
{
	return ! Match(str, 0, str.GetByteCount(), kIgnoreSubmatches).IsEmpty();
}

/******************************************************************************
 Match

 *****************************************************************************/

inline JStringMatch
JRegex::Match
	(
	const JString&			str,
	const IncludeSubmatches	includeSubmatches
	)
	const
{
	return Match(str, 0, str.GetByteCount(), includeSubmatches);
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
	return Match(str, byteIndex-1, str.GetByteCount(), kIncludeSubmatches);
}

/******************************************************************************
 GetSubexpressionIndex

 *****************************************************************************/

inline bool
JRegex::GetSubexpressionIndex
	(
	const JString& name,
	JIndex*        index
	)
	const
{
	return GetSubexpressionIndex(name.GetBytes(), index);
}

/******************************************************************************
 RawGetOption (private)

 *****************************************************************************/

inline bool
JRegex::RawGetOption
	(
	const int flags,
	const int option
	)
	const
{
	return flags & option;
}

#endif
