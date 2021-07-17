#ifndef _H_CBTCLScanner
#define _H_CBTCLScanner

/******************************************************************************
 CBTCLScanner.h

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBTCLScannerL
#undef yyFlexLexer
#define yyFlexLexer CBTCLFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class CBTCLScanner : public CBTCLFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBTCLStyler

	enum TokenType
	{
		kEOF = 258,

		kEmptyVariable,
		kUnterminatedString,

		kWhitespace,	// must be the one before the first item in type table

		kPredefinedWord,
		kVariable,
		kString,
		kComment,

		kBrace,
		kSquareBracket,
		kParenthesis,
		kSemicolon,

		kOtherWord,
		kError			// place holder for CBTCLStyler
	};

	struct Token
	{
		TokenType				type;
		JStyledText::TextRange	range;

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

	CBTCLScanner();

	virtual ~CBTCLScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

private:

	bool				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBTCLScanner(const CBTCLScanner& source);
	const CBTCLScanner& operator=(const CBTCLScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBTCLScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBTCLScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBTCLScanner::Token
CBTCLScanner::ThisToken
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
	const CBTCLScanner::Token& t1,
	const CBTCLScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBTCLScanner::kEOF));
}

inline int
operator!=
	(
	const CBTCLScanner::Token& t1,
	const CBTCLScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
