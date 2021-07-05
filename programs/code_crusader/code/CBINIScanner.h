#ifndef _H_CBINIScanner
#define _H_CBINIScanner

/******************************************************************************
 CBINIScanner.h

	Copyright © 2013 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBINIScannerL
#undef yyFlexLexer
#define yyFlexLexer CBINIFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class CBINIScanner : public CBINIFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBINIStyler

	enum TokenType
	{
		kEOF = 258,

		kUnterminatedString,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kSectionName,

		kKeyOrValue,
		kAssignment,
		kArrayIndex,

		kSingleQuoteString,
		kDoubleQuoteString,

		kComment,

		kError			// place holder for CBINIStyler
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

	CBINIScanner();

	virtual ~CBINIScanner();

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

	CBINIScanner(const CBINIScanner& source);
	const CBINIScanner& operator=(const CBINIScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBINIScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBINIScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBINIScanner::Token
CBINIScanner::ThisToken
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
	const CBINIScanner::Token& t1,
	const CBINIScanner::Token& t2
	)
{
	return (t1.type == t2.type &&
			(t1.range.charRange == t2.range.charRange ||
			 t1.type == CBINIScanner::kEOF));
}

inline int
operator!=
	(
	const CBINIScanner::Token& t1,
	const CBINIScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
