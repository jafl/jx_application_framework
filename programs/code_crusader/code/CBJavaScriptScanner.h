#ifndef _H_CBJavaScriptScanner
#define _H_CBJavaScriptScanner

/******************************************************************************
 CBJavaScriptScanner.h

	Copyright (C) 2006 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CBJavaScriptScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaScriptFlexLexer
#include <FlexLexer.h>
#endif

#include <JIndexRange.h>

class JString;

class CBJavaScriptScanner : public CBJavaScriptFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBJavaScriptStyler

	enum TokenType
	{
		kEOF = 258,

		kBadHexInt,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kString,
		kTemplateString,

		kFloat,
		kDecimalInt,
		kHexInt,

		kRegexSearch,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBJavaScriptStyler
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

	CBJavaScriptScanner();

	virtual ~CBJavaScriptScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;
	JBoolean	itsProbableOperatorFlag;	// kTrue if / is most likely operator instead of regex

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	DocToken(const TokenType type);

	// not allowed

	CBJavaScriptScanner(const CBJavaScriptScanner& source);
	const CBJavaScriptScanner& operator=(const CBJavaScriptScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBJavaScriptScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBJavaScriptScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBJavaScriptScanner::Token
CBJavaScriptScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 DocToken (private)

 *****************************************************************************/

inline CBJavaScriptScanner::Token
CBJavaScriptScanner::DocToken
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
	const CBJavaScriptScanner::Token& t1,
	const CBJavaScriptScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBJavaScriptScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBJavaScriptScanner::Token& t1,
	const CBJavaScriptScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
