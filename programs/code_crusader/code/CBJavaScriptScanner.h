#ifndef _H_CBJavaScriptScanner
#define _H_CBJavaScriptScanner

/******************************************************************************
 CBJavaScriptScanner.h

	Copyright © 2006 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBJavaScriptScannerL
#undef yyFlexLexer
#define yyFlexLexer CBJavaScriptFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

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

	CBJavaScriptScanner();

	virtual ~CBJavaScriptScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;
	JBoolean				itsProbableOperatorFlag;	// kTrue if / is most likely operator instead of regex

private:

	void	InitToken();
	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	DocToken(const TokenType type);

	// not allowed

	CBJavaScriptScanner(const CBJavaScriptScanner& source);
	const CBJavaScriptScanner& operator=(const CBJavaScriptScanner& source);
};


/******************************************************************************
 InitToken (private)

 *****************************************************************************/

inline void
CBJavaScriptScanner::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBJavaScriptScanner::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBJavaScriptScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
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
	const CBJavaScriptScanner::Token& t1,
	const CBJavaScriptScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBJavaScriptScanner::kEOF));
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
