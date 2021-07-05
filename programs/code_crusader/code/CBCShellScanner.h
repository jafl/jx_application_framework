#ifndef _H_CBCShellScanner
#define _H_CBCShellScanner

/******************************************************************************
 CBCShellScanner.h

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBCShellScannerL
#undef yyFlexLexer
#define yyFlexLexer CBCShellFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class CBCShellScanner : public CBCShellFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBCShellStyler

	enum TokenType
	{
		kEOF = 258,

		kEmptyVariable,
		kUnterminatedString,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kVariable,
		kReservedWord,
		kBuiltInCommand,

		kControlOperator,
		kRedirectionOperator,
		kHistoryOperator,
		kMathOperator,

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for CBCShellStyler

		kOtherOperator
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

	CBCShellScanner();

	virtual ~CBCShellScanner();

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

	CBCShellScanner(const CBCShellScanner& source);
	const CBCShellScanner& operator=(const CBCShellScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBCShellScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBCShellScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBCShellScanner::Token
CBCShellScanner::ThisToken
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
	const CBCShellScanner::Token& t1,
	const CBCShellScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBCShellScanner::kEOF));
}

inline int
operator!=
	(
	const CBCShellScanner::Token& t1,
	const CBCShellScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
