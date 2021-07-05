#ifndef _H_CBPythonScanner
#define _H_CBPythonScanner

/******************************************************************************
 CBPythonScanner.h

	Copyright © 1997 by Dustin Laurence.
	Copyright © 2004 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBPythonScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPythonFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class CBPythonScanner : public CBPythonFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBPythonStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kUnterminatedString,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kString,

		kFloat,
		kDecimalInt,
		kHexInt,
		kOctalInt,
		kImaginary,

		kComment,

		kError,			// place holder for CBPythonStyler

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

	CBPythonScanner();

	virtual ~CBPythonScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

protected:

	void	Undo(const JStyledText::TextRange& range,
				 const JSize prevCharByteCount, const JString& text);

private:

	JBoolean				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;
	JBoolean				itsDoubleQuoteFlag;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBPythonScanner(const CBPythonScanner& source);
	const CBPythonScanner& operator=(const CBPythonScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBPythonScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBPythonScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBPythonScanner::Token
CBPythonScanner::ThisToken
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
	const CBPythonScanner::Token& t1,
	const CBPythonScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBPythonScanner::kEOF));
}

inline int
operator!=
	(
	const CBPythonScanner::Token& t1,
	const CBPythonScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
