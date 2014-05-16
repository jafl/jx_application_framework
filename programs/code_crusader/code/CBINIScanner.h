#ifndef _H_CBINIScanner
#define _H_CBINIScanner

/******************************************************************************
 CBINIScanner.h

	Copyright © 2013 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBINIScannerL
#undef yyFlexLexer
#define yyFlexLexer CBINIFlexLexer
#include <FlexLexer.h>
#endif

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JString.h>

class JString;

class CBINIScanner : public CBINIFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBINIStyler

	enum TokenType
	{
		kEOF = 258,

		kUnterminatedString,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kSectionName,

		kKeyOrValue,
		kAssignment,
		kArrayIndex,

		kSingleQuoteString,
		kDoubleQuoteString,

		kComment,

		kError			// place holder for CBINIStyler
	};

	struct Token
	{
		TokenType	type;
		JIndexRange	range;

		Token()
			:
			type(kEOF), range()
			{ };

		Token(const TokenType t, const JIndexRange& r)
			:
			type(t), range(r)
			{ };
	};

public:

	CBINIScanner();

	virtual ~CBINIScanner();

	void	BeginScan(istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBINIScanner(const CBINIScanner& source);
	const CBINIScanner& operator=(const CBINIScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBINIScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBINIScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBINIScanner::Token
CBINIScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 Comparisons

	These compare the type and *range*, not the contents, since ranges do not
	know where they came from.  Still useful when the tokens come from the same
	string.

 *****************************************************************************/

inline int
operator==
	(
	const CBINIScanner::Token& t1,
	const CBINIScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
				   t1.range == t2.range || t1.type == CBINIScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBINIScanner::Token& t1,
	const CBINIScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
