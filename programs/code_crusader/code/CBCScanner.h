#ifndef _H_CBCScanner
#define _H_CBCScanner

/******************************************************************************
 CBCScanner.h

	Copyright © 1997 by Dustin Laurence.
	Copyright © 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBCScannerL
#undef yyFlexLexer
#define yyFlexLexer CBCFlexLexer
#include <FlexLexer.h>
#endif

#include <JStyledText.h>

class JString;

class CBCScanner : public CBCFlexLexer
{
public:

	// these types are ordered to correspond to the type table in CBCStyler

	enum TokenType
	{
		kEOF = 258,

		kBadInt,
		kBadCharConst,
		kUnterminatedString,
		kUnterminatedCComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kDollarID,
		kReservedCKeyword,
		kReservedCPPKeyword,
		kBuiltInDataType,

		kOperator,
		kDelimiter,

		kString,
		kCharConst,

		kFloat,
		kDecimalInt,
		kHexInt,
		kOctalInt,

		kComment,
		kPPDirective,

		kTrigraph,
		kRespelling,
		kError,			// place holder for CBCStyler

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

	CBCScanner();

	virtual ~CBCScanner();

	void	BeginScan(const JStyledText::TextIndex& startIndex, std::istream& input);
	Token	NextToken();		// written by flex

	const JStyledText::TextRange&	GetPPNameRange() const;

protected:

	void	Undo(const JStyledText::TextRange& range,
				 const JSize prevCharByteCount, const JString& text);

private:

	bool				itsResetFlag;
	JStyledText::TextRange	itsCurrentRange;
	JStyledText::TextRange	itsPPNameRange;

private:

	void	StartToken();
	void	ContinueToken();
	Token	ThisToken(const TokenType type);

	void	SavePPNameRange();

	// not allowed

	CBCScanner(const CBCScanner& source);
	const CBCScanner& operator=(const CBCScanner& source);
};


/******************************************************************************
 StartToken (private)

 *****************************************************************************/

inline void
CBCScanner::StartToken()
{
	itsCurrentRange.charRange.SetToEmptyAt(itsCurrentRange.charRange.last+1);
	itsCurrentRange.byteRange.SetToEmptyAt(itsCurrentRange.byteRange.last+1);
	ContinueToken();
}

/******************************************************************************
 ContinueToken (private)

 *****************************************************************************/

inline void
CBCScanner::ContinueToken()
{
	itsCurrentRange.charRange.last += JString::CountCharacters(yytext, yyleng);
	itsCurrentRange.byteRange.last += yyleng;
}

/******************************************************************************
 ThisToken (private)

 *****************************************************************************/

inline CBCScanner::Token
CBCScanner::ThisToken
	(
	const TokenType type
	)
{
	return Token(type, itsCurrentRange);
}

/******************************************************************************
 GetPPNameRange

	Saves the range containing the name of the preprocessor directive.

	e.g.  ...\n  #include <iostream>  // need to read from std::cin\n...
				 ^^^^^^^^

	This is necessary because the entire line is treated as one token
	in order to simplify the context-sensitive highlighting code.

 *****************************************************************************/

inline const JStyledText::TextRange&
CBCScanner::GetPPNameRange()
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
	const CBCScanner::Token& t1,
	const CBCScanner::Token& t2
	)
{
	return ( t1.type == t2.type
			 &&
				(
					t1.range.charRange == t2.range.charRange || t1.type == CBCScanner::kEOF
				)
		   );
}

inline int
operator!=
	(
	const CBCScanner::Token& t1,
	const CBCScanner::Token& t2
	)
{
	return !(t1 == t2);
}

#endif
