#ifndef _H_CBPerlScanner
#define _H_CBPerlScanner

/******************************************************************************
 CBPerlScanner.h

	Copyright (C) 2003 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBPerlScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPerlFlexLexer
#include <FlexLexer.h>
#endif

#include <JString.h>

class JString;

class CBPerlScanner : public CBPerlFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBPerlStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kBadBinary,
		kBadHex,
		kEmptyVariable,
		kUnterminatedVariable,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedWordList,
		kUnterminatedTransliteration,
		kUnterminatedFileGlob,
		kUnterminatedPOD,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kScalar,
		kList,
		kHash,
		kSubroutine,
		kPrototypeArgList,
		kReference,
		kReservedKeyword,
		kBareword,

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
		kVersion,

		kRegexSearch,
		kRegexReplace,
		kOneShotRegexSearch,
		kCompiledRegex,
		kTransliteration,

		kFileGlob,

		kComment,
		kPOD,
		kPPDirective,
		kModuleData,

		kError			// place holder for CBPerlStyler
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

	CBPerlScanner();

	virtual ~CBPerlScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

	const JIndexRange&	GetPPNameRange() const;

private:

	JBoolean	itsResetFlag;
	JIndexRange	itsCurrentRange;
	JIndexRange	itsPPNameRange;
	JBoolean	itsProbableOperatorFlag;	// kTrue if /,? are most likely operators instead of regex
	TokenType	itsComplexVariableType;
	JString		itsHereDocTag;
	TokenType	itsHereDocType;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	JBoolean	SlurpQuoted(const JSize count, const JCharacter* suffixList);

	void	SavePPNameRange();

	// not allowed

	CBPerlScanner(const CBPerlScanner& source);
	const CBPerlScanner& operator=(const CBPerlScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBPerlScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBPerlScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBPerlScanner::Token
CBPerlScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 GetPPNameRange

	Saves the range containing the name of the preprocessor directive.

	e.g.  ...\n  #line 200 "foo"\n...
				 ^^^^^

	This is necessary because the entire line is treated as one token
	in order to simplify the context-sensitive highlighting code.

 *****************************************************************************/

inline const JIndexRange&
CBPerlScanner::GetPPNameRange()
	const
{
	return itsPPNameRange;
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
	const CBPerlScanner::Token& t1,
	const CBPerlScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBPerlScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBPerlScanner::Token& t1,
	const CBPerlScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
