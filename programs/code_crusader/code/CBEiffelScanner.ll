%top {
/*
Copyright © 2004 by John Lindal.

This scanner reads an Eiffle file and returns CBEiffelScanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::Eiffel" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBEiffelStyler

	enum TokenType
	{
		kBadDecimalInt = kEOF+1,
		kUnterminatedString,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,

		kOperator,
		kFreeOperator,
		kDelimiter,

		kString,

		kFloat,
		kDecimalInt,
		kBinary,

		kComment,

		kError			// place holder for CBEiffelStyler
	};
}

%x STRING_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD     (alias|all|and|as|check|class|create|creation|current|debug|deferred|do|else|elseif|end|ensure|expanded|export|external|false|feature|from|frozen|if|implies|indexing|infix|inherit|inspect|invariant|is|like|local|loop|not|obsolete|old|once|or|prefix|redefine|rename|require|rescue|result|retry|select|separate|strip|then|true|undefine|unique|until|variant|when|xor)

DATATYPE    (BIT|BOOLEAN|CHARACTER|DOUBLE|INTEGER|NONE|POINTER|REAL|STRING)



IDCAR        (_|\p{L})
IDCDR        (_|\p{L}|\d)

ID           ({IDCAR}{IDCDR}*)


%{
	/* Agrees with Harbison & Steele's BNF */
%}
DECIMAL      ([0-9]+|[0-9]{1,3}(_[0-9]{3})*)
BADDECIMAL   ([0-9_]+)
BINARY       ([01]+[bB])


%{
	/* Following Harbison & Steele's BNF, except of course I'm using regexes */
%}
SIGNPART     ([+-])
DIGITSEQ     {DECIMAL}
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)

%%



	/* Match operators */



"+"   |
"-"   |
"*"   |
"/"   |
"<"   |
">"   |
"//"  |
\\\\  |
">="  |
"<="  |
"^"   |
"%C"  |
"="   |
"/="  |
"?="  |
":="  |
"<<"  |
">>"  |
"->"  |
":"   |
"$"   |
"%D"  |
"."   |
".."  |
"!"   {
	StartToken();
	return ThisToken(kOperator);
	}

([@#|&]|%A|%S|%V){ID} {
	StartToken();
	return ThisToken(kFreeOperator);
	}



	/* Match grouping symbols and other delimiters */



"("  |
")"  |
"["  |
"]"  |
\%\( |
\%\) |
"{"  |
"}"  |
\%\< |
\%\> |
","  |
";"  {
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

{ID} {
	StartToken();
	return ThisToken(kID);
	}

{DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{BADDECIMAL} {
	StartToken();
	return ThisToken(kBadDecimalInt);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{BINARY} {
	StartToken();
	return ThisToken(kBinary);
	}



	/* Match comments */



"--".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* The following match the tokens that are too dangerous to eat in one     */
	/* bite because it is so easy to create unterminated comments and strings. */



	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */



"%"?\" {
	StartToken();
	start(STRING_STATE);
	}

<STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kString);
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

"%"(.|[ \t]*\n[ \t]*%)? |
[^%\n\"]+               {
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
