#ifndef _H_CBEiffelScanner
#define _H_CBEiffelScanner

/******************************************************************************
 CBEiffelScanner.h

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2004 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBEiffelScannerL
#undef yyFlexLexer
#define yyFlexLexer CBEiffelFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

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

		kError			// place holder for CBEiffelStyler
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

	CBEiffelScanner();

	virtual ~CBEiffelScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

protected:

	void	Undo(const JStyledText::TextRange& range,
				 const JSize prevCharByteCount, const JString& text);

private:

	bool				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;

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
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBEiffelScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
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
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBEiffelScanner::kEOF));
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
