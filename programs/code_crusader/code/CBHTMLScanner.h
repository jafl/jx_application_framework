#ifndef _H_CBHTMLScanner
#define _H_CBHTMLScanner

/******************************************************************************
 CBHTMLScanner.h

	Copyright © 2001-2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBHTMLScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPHPFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>
#include <JPtrArray-JString.h>
#include <JPtrStack.h>

class CBHTMLScanner : public CBPHPFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBHTMLStyler

	enum TokenType
	{
		kEOF = 258,

		kHTMLEmptyTag,
		kHTMLUnterminatedTag,
		kHTMLInvalidNamedCharacter,
		kUnterminatedString,
		kUnterminatedComment,
		kUnterminatedPHPVariable,
		kUnterminatedJSRegex,
		kJavaBadCharConst,
		kPHPBadInt,
		kBadHex,
		kIllegalChar,

		kWhitespace,		// must be the one before the first item in type table

		// HTML

		kHTMLText,
		kHTMLTag,
		kHTMLScript,
		kHTMLNamedCharacter,
		kHTMLComment,
		kCDATABlock,

		// Mustache

		kMustacheCommand,

		// PHP

		kPHPStartEnd,
		kPHPID,
		kPHPVariable,
		kPHPReservedKeyword,
		kPHPBuiltInDataType,

		kPHPOperator,
		kPHPDelimiter,

		kPHPSingleQuoteString,
		kPHPDoubleQuoteString,
		kPHPHereDocString,
		kPHPNowDocString,
		kPHPExecString,

		// JSP/Java

		kJSPStartEnd,
		kJSPComment,

		kJavaID,
		kJavaReservedKeyword,
		kJavaBuiltInDataType,

		kJavaOperator,
		kJavaDelimiter,

		kJavaString,
		kJavaCharConst,

		// JavaScript

		kJSID,
		kJSReservedKeyword,

		kJSOperator,
		kJSDelimiter,

		kJSString,
		kJSTemplateString,
		kJSRegexSearch,

		// shared

		kFloat,
		kDecimalInt,
		kHexInt,
		kPHPOctalInt,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBHTMLStyler
	};

	struct Token
	{
		TokenType				type;
		JStyledText::TextRange	range;
		const JString*			language;			// if type == kHTMLScript
		JStyledText::TextRange	docCommentRange;	// preceding comment range for DOC comment tags

		Token()
			:
			type(kEOF), language(nullptr)
			{ };

		Token(const TokenType t, const JStyledText::TextRange& r, const JString* l)
			:
			type(t), range(r), language(l)
			{ };
	};

public:

	CBHTMLScanner();

	virtual ~CBHTMLScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex,
					  std::istream& input, const yy_state_type startState);
	Token	NextToken();		// written by flex

protected:

	yy_state_type	GetCurrentLexerState() const;
	JBoolean		InTagState() const;

	virtual const JString&	GetScannedText() const = 0;

private:

	JBoolean				itsResetFlag;
	yy_state_type			itsStartState;
	JStyledText::TextRange	itsCurrentRange;
	JString					itsScriptLanguage;
	JString					itsPHPHereDocTag;
	JString					itsPHPNowDocTag;
	JBoolean				itsProbableJSOperatorFlag;	// kTrue if / is most likely operator instead of regex

private:

	void	InitToken();
	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);
	Token	ScriptToken();
	Token	DocToken(const TokenType type);

	JBoolean	IsScript(JString* language) const;
	JBoolean	InTagState(const int state) const;

	// not allowed

	CBHTMLScanner(const CBHTMLScanner& source);
	const CBHTMLScanner& operator=(const CBHTMLScanner& source);
};


/******************************************************************************
 InitToken (private)

 *****************************************************************************/

inline void
CBHTMLScanner::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBHTMLScanner::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBHTMLScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange, nullptr);
}

/******************************************************************************
 ScriptToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::ScriptToken()
{
	return Token(kHTMLScript, itsCurrentRange, &itsScriptLanguage);
}

/******************************************************************************
 DocToken (private)

 *****************************************************************************/

inline CBHTMLScanner::Token
CBHTMLScanner::DocToken
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
	const CBHTMLScanner::Token& t1,
	const CBHTMLScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange || t1.type == CBHTMLScanner::kEOF));
}

inline int
operator!=
	(
	const CBHTMLScanner::Token& t1,
	const CBHTMLScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
