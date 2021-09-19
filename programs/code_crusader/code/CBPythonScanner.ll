%top {
/*
Copyright © 2004 by John Lindal.

This scanner reads a Python file and returns CB::Text::Python::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::Python" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBPythonStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
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

private:

	bool	itsDoubleQuoteFlag;
}

%x SHORT_STRING_STATE LONG_STRING_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD      (and|access|break|class|continue|def|del|elif|else|except|exec|finally|for|from|global|if|import|in|is|lambda|not|or|pass|print|raise|return|try|while|yield)



IDCAR        (_|\p{L})
IDCDR        (_|\p{L}|\d)

ID           ({IDCAR}{IDCDR}*)


%{
	/* Agrees with Harbison & Steele's BNF */
%}
INTSUFFIX    ([lL])

DECIMAL      ((0|[1-9][0-9]*){INTSUFFIX}?)
HEX          (0[xX][[:xdigit:]]*{INTSUFFIX}?)
OCTAL        (0[0-7]+{INTSUFFIX}?)
%{
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
%}
BADINT       ([0-9]+{INTSUFFIX}?)
%{
	/* We can't define BADHEX because 0xAAU is legal while 0xAUA isn't */
	/* and regex subexpressions are greedy. */
%}

%{
	/* Following Harbison & Steele's BNF, except of course I'm using regexes */
%}
SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)



IMAGSUFFIX    ([jJ])

IMAG          ({DIGITSEQ}|{FLOAT}){IMAGSUFFIX}

%%



	/* Match misc. legal symbols */



\\ { /* Display separately when not part of a string */
	StartToken();
	return ThisToken(kContinuation);
	}



	/* Match operators */



"+"   |
"-"   |
"*"   |
"**"  |
"/"   |
"//"  |
"%"   |
"<<"  |
">>"  |
"&"   |
"|"   |
"^"   |
"~"   |
">"   |
"<"   |
"<="  |
">="  |
"=="  |
"!="  |
"<>"  |
"."   |
"="   |
"+="  |
"-="  |
"*="  |
"/="  |
"//=" |
"%="  |
"&="  |
"|="  |
"^="  |
">>=" |
"<<=" |
"**=" {
	StartToken();
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"("   |
")"   |
"["   |
"]"   |
"{"   |
"}"   |
","   |
":"   |
"..." |
";"   {
	StartToken();
	return ThisToken(kDelimiter);
	}



	/* Match normal tokens */



{KEYWORD} {
	StartToken();
	return ThisToken(kReservedKeyword);
	}

{ID} {
	StartToken();
	return ThisToken(kID);
	}

{DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{OCTAL} {
	StartToken();
	return ThisToken(kOctalInt);
	}

{BADINT} {
	StartToken();
	return ThisToken(kBadInt);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{IMAG} {
	StartToken();
	return ThisToken(kImaginary);
	}



	/* Match comments */



"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* The following match the tokens that are too dangerous to eat in one     */
	/* bite because it is so easy to create unterminated comments and strings. */



	/* Match short strings -- cannot span lines without \ */



[uU]?[rR]?\" {
	StartToken();
	itsDoubleQuoteFlag = true;
	start(SHORT_STRING_STATE);
	}

[uU]?[rR]?\' {
	StartToken();
	itsDoubleQuoteFlag = false;
	start(SHORT_STRING_STATE);
	}

<SHORT_STRING_STATE>{

\" {
	ContinueToken();
	if (itsDoubleQuoteFlag)
		{
		start(INITIAL);
		return ThisToken(kString);
		}
	}

\' {
	ContinueToken();
	if (!itsDoubleQuoteFlag)
		{
		start(INITIAL);
		return ThisToken(kString);
		}
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?    |
[^\\\n\"\']+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match long strings -- can span lines */



[uU]?[rR]?\"\"\" {
	StartToken();
	itsDoubleQuoteFlag = true;
	start(LONG_STRING_STATE);
	}

[uU]?[rR]?\'\'\' {
	StartToken();
	itsDoubleQuoteFlag = false;
	start(LONG_STRING_STATE);
	}

<LONG_STRING_STATE>{

\"\"\" {
	ContinueToken();
	if (itsDoubleQuoteFlag)
		{
		start(INITIAL);
		return ThisToken(kString);
		}
	}

\'\'\' {
	ContinueToken();
	if (!itsDoubleQuoteFlag)
		{
		start(INITIAL);
		return ThisToken(kString);
		}
	}

\"         |
\'         |
\\(.|\n)?  |
[^\\\"\']+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Clean up */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end or we'd miss CPP directives */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	JUtf8Character c(text());
	if (c.IsPrint())
		{
		return ThisToken(kIllegalChar);
		}
	else
		{
		return ThisToken(kNonPrintChar);
		}
	}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
