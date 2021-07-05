#ifndef _H_CBGoScanner
#define _H_CBGoScanner

/******************************************************************************
 CBGoScanner.h

	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBGoScannerL
#undef yyFlexLexer
#define yyFlexLexer CBGoFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class JString;

class CBGoScanner : public CBGoFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBGoStyler

	enum TokenType
	{
		kEOF = 258,

		kBadRune,
		kUnterminatedString,
		kUnterminatedRawString,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,
		kBuiltInFunction,

		kOperator,
		kDelimiter,

		kString,
		kRawString,
		kRune,

		kFloat,
		kHexFloat,
		kDecimalInt,
		kBinaryInt,
		kOctalInt,
		kHexInt,
		kImaginary,

		kComment,

		kError			// place holder for CBGoStyler
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

	CBGoScanner();

	virtual ~CBGoScanner();

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

	CBGoScanner(const CBGoScanner& source);
	const CBGoScanner& operator=(const CBGoScanner& source);
};


/******************************************************************************
 InitToken (private)

 *****************************************************************************/

inline void
CBGoScanner::InitToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
}

/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBGoScanner::StartToken()
{
	InitToken();
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBGoScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBGoScanner::Token
CBGoScanner::ThisToken
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
	const CBGoScanner::Token& t1,
	const CBGoScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBGoScanner::kEOF));
}

inline int
operator!=
	(
	const CBGoScanner::Token& t1,
	const CBGoScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
