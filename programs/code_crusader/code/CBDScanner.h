#ifndef _H_CBDScanner
#define _H_CBDScanner

/******************************************************************************
 CBDScanner.h

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2021 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBDScannerL
#undef yyFlexLexer
#define yyFlexLexer CBDFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class JString;

class CBDScanner : public CBDFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBDStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kBadCharConst,
		kUnterminatedString,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,

		kOperator,
		kDelimiter,

		kString,
		kWysiwygString,
		kHexString,
		kTokenString,
		kCharConst,

		kFloat,
		kHexFloat,
		kDecimalInt,
		kBinaryInt,
		kHexInt,

		kComment,

		kError,			// place holder for CBDStyler

		// special cases

		kContinuation
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

	CBDScanner();

	virtual ~CBDScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBDScanner(const CBDScanner& source);
	const CBDScanner& operator=(const CBDScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBDScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBDScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBDScanner::Token
CBDScanner::ThisToken
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
	const CBDScanner::Token& t1,
	const CBDScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range.charRange == t2.range.charRange || t1.type == CBDScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBDScanner::Token& t1,
	const CBDScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
