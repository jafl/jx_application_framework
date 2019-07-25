#ifndef _H_CBJavaScanner
#define _H_CBJavaScanner

/******************************************************************************
 CBJavaScanner.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBJavaScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

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
		TokenType				type;
		JStyledText::TextRange	range;
		JStyledText::TextRange	docCommentRange;	// preceding comment range for DOC comment tags

		Token()
			:
			type(kEOF)
			{ };

		Token(const TokenType t, const JStyledText::TextRange& r)
			:
			type(t), range(r)
			{ };
	};

public:

	CBJavaScanner();

	virtual ~CBJavaScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;

private:

	void	InitToken();
	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	DocToken(const TokenType type);

	// not allowed

	CBJavaScanner(const CBJavaScanner& source);
	const CBJavaScanner& operator=(const CBJavaScanner& source);
};


/******************************************************************************
 InitToken (private)

 *****************************************************************************/

inline void
CBJavaScanner::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBJavaScanner::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBJavaScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
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
	InitToken();

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
					t1.range.charRange == t2.range.charRange || t1.type == CBJavaScanner::kEOF
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
