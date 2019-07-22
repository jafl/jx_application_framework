#ifndef _H_CBSQLScanner
#define _H_CBSQLScanner

/******************************************************************************
 CBSQLScanner.h

	Copyright (C) 2017 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBSQLScannerL
#undef yyFlexLexer
#define yyFlexLexer CBSQLFlexLexer
#include <FlexLexer.h>
#endif

#include <JUtf8ByteRange.h>

class CBSQLScanner : public CBSQLFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBSQLStyler

	enum TokenType
	{
		kEOF = 258,

		kUnterminatedString,
		kUnterminatedComment,
		kIllegalChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kVariable,
		kKeyword,
		kReservedWord,
		kBuiltInFunction,
		kExtensionMySQL,
		kExtensionPostgreSQL,
		kExtensionOracle,

		kOperator,
		kDelimiter,
		kMySQLOperator,

		kSingleQuoteString,
		kDoubleQuoteString,
		kBackQuoteString,

		kFloat,
		kDecimalInteger,
		kHexInteger,

		kComment,
		kMySQLComment,
		kError			// place holder for CBSQLStyler
	};

	struct Token
	{
		TokenType		type;
		JUtf8ByteRange	range;

		Token()
			:
			type(kEOF), range()
			{ };

		Token(const TokenType t, const JUtf8ByteRange& r)
			:
			type(t), range(r)
			{ };
	};

public:

	CBSQLScanner();

	virtual ~CBSQLScanner();

	void	BeginScan(std::istream& input);
	Token	NextToken();		// written by flex

private:

	JBoolean		itsResetFlag;
	JUtf8ByteRange	itsCurrentRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	// not allowed

	CBSQLScanner(const CBSQLScanner& source);
	const CBSQLScanner& operator=(const CBSQLScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBSQLScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBSQLScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBSQLScanner::Token
CBSQLScanner::ThisToken
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
	const CBSQLScanner::Token& t1,
	const CBSQLScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBSQLScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBSQLScanner::Token& t1,
	const CBSQLScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
