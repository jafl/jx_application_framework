#ifndef _H_CBPropertiesScanner
#define _H_CBPropertiesScanner

/******************************************************************************
 CBPropertiesScanner.h

	Copyright (C) 2015 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBPropertiesScannerL
#undef yyFlexLexer
#define yyFlexLexer CBPropertiesFlexLexer
#include <FlexLexer.h>
#endif

#include <JString.h>

class JString;

class CBPropertiesScanner : public CBPropertiesFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBPropertiesStyler

	enum TokenType
	{
		kEOF = 258,

		kUnterminatedString,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kKeyOrValue,
		kAssignment,

		kComment,

		kError			// place holder for CBPropertiesStyler
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

	CBPropertiesScanner();

	virtual ~CBPropertiesScanner();

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

	CBPropertiesScanner(const CBPropertiesScanner& source);
	const CBPropertiesScanner& operator=(const CBPropertiesScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBPropertiesScanner::StartToken()
{
	const JIndex prevEnd = itsCurrentRange.last;
	itsCurrentRange.Set(prevEnd+1, prevEnd+yyleng);
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBPropertiesScanner::ContinueToken()
{
	itsCurrentRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBPropertiesScanner::Token
CBPropertiesScanner::ThisToken
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
	const CBPropertiesScanner::Token& t1,
	const CBPropertiesScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range == t2.range || t1.type == CBPropertiesScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBPropertiesScanner::Token& t1,
	const CBPropertiesScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
