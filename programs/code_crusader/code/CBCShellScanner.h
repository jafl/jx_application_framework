#ifndef _H_CBCShellScanner
#define _H_CBCShellScanner

/******************************************************************************
 CBCShellScanner.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBCShellScannerL
#undef yyFlexLexer
#define yyFlexLexer CBCShellFlexLexer
#include <FlexLexer.h>
#endif

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JIndexRange.h>

class CBCShellScanner : public CBCShellFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBCShellStyler

	enum TokenType
	{
		kEOF = 258,

		kEmptyVariable,
		kUnterminatedString,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kVariable,
		kReservedWord,
		kBuiltInCommand,

		kControlOperator,
		kRedirectionOperator,
		kHistoryOperator,
		kMathOperator,

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for CBCShellStyler

		kOtherOperator
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

	CBCShellScanner();

	virtual ~CBCShellScanner();

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

	CBCShellScanner(const CBCShellScanner& source);
	const CBCShellScanner& operator=(const CBCShellScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBCShellScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBCShellScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBCShellScanner::Token
CBCShellScanner::ThisToken
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
	const CBCShellScanner::Token& t1,
	const CBCShellScanner::Token& t2
	)
{
	return ( t1.type == t2.type
	         &&
	            (
	               t1.range == t2.range || t1.type == CBCShellScanner::kEOF
	            )
	       );
}

inline int
operator!=
	(
	const CBCShellScanner::Token& t1,
	const CBCShellScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
