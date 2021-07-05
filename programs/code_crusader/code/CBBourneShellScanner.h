#ifndef _H_CBBourneShellScanner
#define _H_CBBourneShellScanner

/******************************************************************************
 CBBourneShellScanner.h

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBBourneShellScannerL
#undef yyFlexLexer
#define yyFlexLexer CBBourneShellFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class CBBourneShellScanner : public CBBourneShellFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBBourneShellStyler

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

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for CBBourneShellStyler

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

	CBBourneShellScanner();

	virtual ~CBBourneShellScanner();

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

	CBBourneShellScanner(const CBBourneShellScanner& source);
	const CBBourneShellScanner& operator=(const CBBourneShellScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBBourneShellScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBBourneShellScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBBourneShellScanner::Token
CBBourneShellScanner::ThisToken
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
	const CBBourneShellScanner::Token& t1,
	const CBBourneShellScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBBourneShellScanner::kEOF));
}

inline int
operator!=
	(
	const CBBourneShellScanner::Token& t1,
	const CBBourneShellScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
