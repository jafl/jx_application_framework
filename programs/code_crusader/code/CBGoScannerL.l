%{
/*
Copyright © 2021 by John Lindal.

This scanner reads a Go file and returns CBGoScanner::Tokens.

Remember to upgrade CBHTMLScanner, too!
*/

#define _H_CBGoScannerL

#include "CBGoScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBGoScanner::Token CBGoScanner::NextToken()
%}

%option c++ yyclass = "CBGoScanner" prefix = "CBGo"
%option 8bit nodefault noyywrap
%option full ecs align

%x C_COMMENT_STATE STRING_STATE

WS           ([ \v\t\f\r\n]+)



KEYWORD      (break|case|chan|const|continue|default|defer|else|fallthrough|false|for|func|go|goto|if|import|interface|iota|map|nil|package|range|return|select|struct|switch|true|type|var)

DATATYPE     (bool|byte|complex64|complex128|error|float32|float64|int|int8|int16|int32|int64|rune|string|uint|uint8|uint16|uint32|uint64|uintptr)

FUNCTION     (append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover)



IDCAR        ([_[:alpha:]])
IDCDR        ([_[:alnum:]])

ID           ({IDCAR}{IDCDR}*)



HEXDIGITS	 (_?[[:xdigit:]]+)

DECIMAL      ([0-9]+(_?[0-9]+)*)
BINARY       (0[bB](_?[0-1]+)+)
OCTAL        (0[oO](_?[0-7]+)+)
HEX          (0[xX]+(HEXDIGITS))



EXPONENT     ([eE][+-]?{DECIMAL})
DOTDIGITS    ({DECIMAL}\.|{DECIMAL}\.{DECIMAL}|\.{DECIMAL})

FLOAT        ({DECIMAL}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)



HEXEXPONENT  ([pP][+-]?{DECIMAL})
HEXDOTDIGITS ({HEXDIGITS}\.|{HEXDIGITS}\.{HEXDIGITS}|\.{HEXDIGITS})

HEXFLOAT     (0[xX]({HEXDIGITS}|{HEXDOTDIGITS}){EXPONENT})



IMAG         (({DECIMAL}|{BINARY}|{OCTAL}|{HEX}|{FLOAT}|{HEXFLOAT})i)



UESCCODE     (\\(u[[:xdigit:]]{4}|U[[:xdigit:]]{8}))
CESCCODE     ([abfnrtv\\'"])
OESCCODE     ([0-7]{3})
HESCCODE     (x[[:xdigit:]]{2})
ESCCHAR      (\\({CESCCODE}|{OESCCODE}|{HESCCODE}))

RUNE         (\'([^\n'\\]|{ESCCHAR}|{UESCCODE})\')
BADRUNE      (\'[^']*\'?)
%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = false;
		}

%}



	/* Match operators */



"++"   |
"--"   |
"+"    |
"-"    |
"!"    |
"*"    |
"/"    |
"%"    |
"<<"   |
">>"   |
"<"    |
"<="   |
">"    |
">="   |
"=="   |
"!="   |
":="   |
"&"    |
"^"    |
"&^"   |
"|"    |
"&&"   |
"||"   |
":"    |
"="    |
"*="   |
"/="   |
"%="   |
"+="   |
"-="   |
"<<="  |
">>="  |
"&="   |
"^="   |
"&^="  |
"|="   |
"<-"   |
"."    |
"..."  {
	StartToken();
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"," |
";" {
	StartToken();
	return ThisToken(kDelimiter);
	}



	/* Match normal tokens */



{KEYWORD} {
	StartToken();
	return ThisToken(kReservedKeyword);
	}

{DATATYPE} {
	StartToken();
	return ThisToken(kBuiltInDataType);
	}

{FUNCTION} {
	StartToken();
	return ThisToken(kBuiltInFunction);
	}

{ID} {
	StartToken();
	return ThisToken(kID);
	}

{DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{BINARY} {
	StartToken();
	return ThisToken(kBinaryInt);
	}

{OCTAL} {
	StartToken();
	return ThisToken(kOctalInt);
	}

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{HEXFLOAT} {
	StartToken();
	return ThisToken(kHexFloat);
	}

{IMAG} {
	StartToken();
	return ThisToken(kImaginary);
	}

`[^`]+` {
	StartToken();
	return ThisToken(kRawString);
	}

`[^`]+ {
	StartToken();
	return ThisToken(kUnterminatedRawString);
	}

{RUNE} {
	StartToken();
	return ThisToken(kRune);
	}

{BADRUNE} {
	StartToken();
	return ThisToken(kBadRune);
	}



	/* Match C++ comments */



"//".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* The following match tokens that are too dangerous to eat in one bite */
	/* because it is so easy to create unterminated comments and strings.   */



	/* Match C comments -- this is the fast comment scanner from the flex man page, */
	/* since Vern Paxon presumably can optimize such things far better than I can.  */



"/*" {
	StartToken();
	BEGIN(C_COMMENT_STATE);
	}

<C_COMMENT_STATE>{

[^*\n]*\n?      |
"*"+[^*/\n]*\n? {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kComment);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */
	/* Go does not support \\\n to continue a string on the next line.  */



\" {
	StartToken();
	BEGIN(STRING_STATE);
	}

<STRING_STATE>{

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kString);
	}

\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\.?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Clean up */



{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	JUtf8Character c(yytext);
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
