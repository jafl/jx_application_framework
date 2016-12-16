#ifndef _H_CBCSharpScanner
#define _H_CBCSharpScanner

/******************************************************************************
 CBCSharpScanner.h

	Copyright (C) 1997 by Dustin Laurence.
	Copyright (C) 2004 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBCSharpScannerL
#undef yyFlexLexer
#define yyFlexLexer CBCSharpFlexLexer
#include <FlexLexer.h>
#endif

#include <JIndexRange.h>

class JString;

class CBCSharpScanner : public CBCSharpFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBCSharpStyler

	enum TokenType
	{
		kEOF = 258,

		kBadCharConst,
		kUnterminatedString,
		kUnterminatedCComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedCKeyword,
		kBuiltInDataType,

		kOperator,
		kDelimiter,

		kString,
		kCharConst,

		kFloat,
		kDecimalInt,
		kHexInt,

		kComment,
		kDocComment,
		kPPDirective,

		kError,			// place holder for CBCSharpStyler
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

	CBCSharpScanner();

	virtual ~CBCSharpScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

	JIndexRange	GetPPNameRange() const;

protected:

	void	Undo(const JIndexRange& range, const JString& text);

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;
	JIndexRange	itsPPNameRange;
	JBoolean	itsIsDocCommentFlag;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	void	SavePPNameRange();

	// not allowed

	CBCSharpScanner(const CBCSharpScanner& source);
	const CBCSharpScanner& operator=(const CBCSharpScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBCSharpScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBCSharpScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBCSharpScanner::Token
CBCSharpScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 GetPPNameRange

	Saves the range containing the name of the preprocessor directive.

	e.g.  ...\n  #include <iostream>  // need to read from std::cin\n...
	             ^^^^^^^^

	This is necessary because the entire line is treated as one token
	in order to simplify the context-sensitive highlighting code.

 *****************************************************************************/

inline JIndexRange
CBCSharpScanner::GetPPNameRange()
	const
{
	return itsPPNameRange;
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
	const CBCSharpScanner::Token& t1,
	const CBCSharpScanner::Token& t2
	)
{
	return ( t1.type == t2.type
	         &&
	            (
	               t1.range == t2.range || t1.type == CBCSharpScanner::kEOF
	            )
	       );
}

inline int
operator!=
	(
	const CBCSharpScanner::Token& t1,
	const CBCSharpScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
