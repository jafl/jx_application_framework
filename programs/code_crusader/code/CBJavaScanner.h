#ifndef _H_CBJavaScanner
#define _H_CBJavaScanner

/******************************************************************************
 CBJavaScanner.h

	Copyright (C) 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBJavaScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaFlexLexer
#include <FlexLexer.h>
#endif

#include <JIndexRange.h>

class JString;

class CBJavaScanner : public CBJavaFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBJavaStyler

	enum TokenType
	{
		kEOF = 258,

		kBadHexInt,
		kBadCharConst,
		kUnterminatedString,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,
		kAnnotation,

		kOperator,
		kDelimiter,

		kString,
		kCharConst,

		kFloat,
		kDecimalInt,
		kHexInt,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBJavaStyler
		};

	struct Token
	{
		TokenType	type;
		JIndexRange	range;
		JIndexRange	docCommentRange;	// preceding comment range for DOC comment tags

		Token()
			:
			type(kEOF), range(), docCommentRange()
			{ };

		Token(const TokenType t, const JIndexRange& r)
			:
			type(t), range(r), docCommentRange()
			{ };
	};

public:

	CBJavaScanner();

	virtual ~CBJavaScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	DocToken(const TokenType type);

	// not allowed

	CBJavaScanner(const CBJavaScanner& source);
	const CBJavaScanner& operator=(const CBJavaScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBJavaScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBJavaScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBJavaScanner::Token
CBJavaScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 DocToken (private)

 *****************************************************************************/

inline CBJavaScanner::Token
CBJavaScanner::DocToken
	(
	const TokenType type
	)
{
	Token t;
	t.docCommentRange = itsCurrentRange;	// save preceding comment range

	StartToken();							// tag is separate token
	t.type  = type;
	t.range = itsCurrentRange;

	// prepare for continuation of comment (StartToken() with yyleng==0)
	itsCurrentRange.Set(itsCurrentRange.last+1, itsCurrentRange.last);

	return t;
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
	const CBJavaScanner::Token& t1,
	const CBJavaScanner::Token& t2
	)
{
	return ( t1.type == t2.type
	         &&
	            (
	               t1.range == t2.range || t1.type == CBJavaScanner::kEOF
	            )
	       );
}

inline int
operator!=
	(
	const CBJavaScanner::Token& t1,
	const CBJavaScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
