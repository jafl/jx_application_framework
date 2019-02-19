#ifndef _H_CBRubyScanner
#define _H_CBRubyScanner

/******************************************************************************
 CBRubyScanner.h

	Copyright (C) 2003 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBRubyScannerL
#undef yyFlexLexer
#define yyFlexLexer CBRubyFlexLexer
#include <FlexLexer.h>
#endif

#include <JString.h>

class JString;

class CBRubyScanner : public CBRubyFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBRubyStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kBadBinary,
		kBadHex,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedWordList,
		kUnterminatedEmbeddedDoc,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kLocalVariable,
		kInstanceVariable,
		kGlobalVariable,
		kSymbol,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kSingleQuoteString,
		kDoubleQuoteString,
		kHereDocString,
		kExecString,
		kWordList,

		kFloat,
		kDecimalInt,
		kBinaryInt,
		kOctalInt,
		kHexInt,

		kRegex,

		kComment,
		kEmbeddedDoc,

		kError			// place holder for CBRubyStyler
	};

	struct Token
	{
		TokenType	type;
		JIndexRange	range;

		Token()
			:
			type(kEOF), range()
			{ };

		Token(const TokenType t, const JIndexRange& r)
			:
			type(t), range(r)
			{ };
	};

public:

	CBRubyScanner();

	virtual ~CBRubyScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;
	JBoolean	itsProbableOperatorFlag;	// kTrue if /,? are most likely operators instead of regex
	TokenType	itsComplexVariableType;
	JString		itsHereDocTag;
	TokenType	itsHereDocType;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	JBoolean	SlurpQuoted(const JUtf8Byte* suffixList);

	// not allowed

	CBRubyScanner(const CBRubyScanner& source);
	const CBRubyScanner& operator=(const CBRubyScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBRubyScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBRubyScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBRubyScanner::Token
CBRubyScanner::ThisToken
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
	const CBRubyScanner::Token& t1,
	const CBRubyScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBRubyScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBRubyScanner::Token& t1,
	const CBRubyScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
