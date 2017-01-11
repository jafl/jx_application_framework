#ifndef _H_CBPythonScanner
#define _H_CBPythonScanner

/******************************************************************************
 CBPythonScanner.h

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBPythonScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPythonFlexLexer
#include <FlexLexer.h>
#endif

#include <JIndexRange.h>

class JString;

class CBPythonScanner : public CBPythonFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBPythonStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kUnterminatedString,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kString,

		kFloat,
		kDecimalInt,
		kHexInt,
		kOctalInt,
		kImaginary,

		kComment,

		kError,			// place holder for CBPythonStyler

		// special cases

		kContinuation
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

	CBPythonScanner();

	virtual ~CBPythonScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

protected:

	void	Undo(const JIndexRange& range, const JString& text);

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;
	JBoolean	itsDoubleQuoteFlag;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBPythonScanner(const CBPythonScanner& source);
	const CBPythonScanner& operator=(const CBPythonScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBPythonScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBPythonScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBPythonScanner::Token
CBPythonScanner::ThisToken
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
	const CBPythonScanner::Token& t1,
	const CBPythonScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBPythonScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBPythonScanner::Token& t1,
	const CBPythonScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
