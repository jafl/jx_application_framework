#ifndef _H_CBEiffelScanner
#define _H_CBEiffelScanner

/******************************************************************************
 CBEiffelScanner.h

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBEiffelScannerL
#undef yyFlexLexer
#define yyFlexLexer CBEiffelFlexLexer
#include <FlexLexer.h>
#endif

#include <JIndexRange.h>

class JString;

class CBEiffelScanner : public CBEiffelFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBEiffelStyler

	enum TokenType
	{
		kEOF = 258,

		kBadDecimalInt,
		kUnterminatedString,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,

		kOperator,
		kFreeOperator,
		kDelimiter,

		kString,

		kFloat,
		kDecimalInt,
		kBinary,

		kComment,

		kError,			// place holder for CBEiffelStyler

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

	CBEiffelScanner();

	virtual ~CBEiffelScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

protected:

	void	Undo(const JIndexRange& range, const JString& text);

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBEiffelScanner(const CBEiffelScanner& source);
	const CBEiffelScanner& operator=(const CBEiffelScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBEiffelScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBEiffelScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBEiffelScanner::Token
CBEiffelScanner::ThisToken
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
	const CBEiffelScanner::Token& t1,
	const CBEiffelScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBEiffelScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBEiffelScanner::Token& t1,
	const CBEiffelScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
