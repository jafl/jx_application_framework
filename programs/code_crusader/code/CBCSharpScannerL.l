%{
/*
Copyright (C) 2004 by John Lindal.

This scanner reads a C# file and returns CBCSharpScanner::Tokens.
*/

#define _H_CBCSharpScannerL

#include "CBCSharpScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBCSharpScanner::Token CBCSharpScanner::NextToken()
%}

%option c++ yyclass = "CBCSharpScanner" prefix = "CBCSharp"
%option 8bit nodefault noyywrap
%option full ecs align

%x C_COMMENT_STATE STRING_STATE SIMPLE_STRING_STATE PP_ARG_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



CKEYWORD     (abstract|as|base|break|case|catch|checked|class|const|continue|default|delegate|do|else|enum|explicit|extern|false|finally|for|foreach|goto|if|implicit|in|interface|internal|is|lock|namespace|new|null|operator|out|override|params|private|protected|public|readonly|ref|return|sealed|sizeof|stackalloc|static|struct|switch|this|throw|true|try|typeof|unchecked|unsafe|using|virtual|volatile|while)

PPKEYWORD    (if|else|elif|endif|define|undef|warning|error|line|region|endregion)

CDATATYPE    (bool|byte|char|decimal|double|event|fixed|float|int|long|object|sbyte|short|string|uint|ulong|ushort|void)



HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)



IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})

ID           @?({IDCAR}{IDCDR}*)



	/* Agrees with Harbison & Steele's BNF */
INTSUFFIX    (([lL][uU]?)|([uU][lL]?))

DECIMAL      ([0-9]+{INTSUFFIX}?)
HEX          (0[xX][[:xdigit:]]+{INTSUFFIX}?)
	/* We can't define BADHEX because 0xAAU is legal while 0xAUA isn't */
	/* and regex subexpressions are greedy. */



	/* Following Harbison & Steele's BNF, except of course I'm using regexes */
FLOATSUFFIX  ([fFdDmM])
SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}{FLOATSUFFIX}?|{DOTDIGITS}{EXPONENT}?{FLOATSUFFIX}?)



	/* Considerably modified from Harbison & Steele to distinguish ANSI multi-byte  */
	/* numeric escape sequences, since they are non-portable and hazardous.  We can */
	/* warn about ANSI brain-damage with a different color/font. */
CESCCODE     ([abfnrtv\\'"])
HESCCODE     ([xX]0*[[:xdigit:]]{1,4})

ESCCHAR      (\\({CESCCODE}|{HESCCODE}))

BADCESCCODE  ([^abfnrtv\\'"])
BADHESCCODE  ([xX]0*[[:xdigit:]]{5,})

BADESCCHAR   (\\({BADCESCCODE}|{BADHESCCODE}))



	/* Again, I'm treating ANSI multi-byte character constants separately */
CCHAR        ([^\n'\\]|{ESCCHAR}|{UESCCODE})

CCONST       (\'{CCHAR}\')
BADCCONST    (\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CCHAR}){2,})\')

%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



	/* Match operators */



":"   |
"?"   |
"="   |
"+="  |
"-="  |
"*="  |
"/="  |
"%="  |
"<<=" |
">>=" |
"&="  |
"^="  |
"|="  |
"<<"  |
">>"  |
"++"  |
"--"  |
">"   |
"<"   |
"<="  |
">="  |
"!"   |
"=="  |
"!="  |
"&&"  |
"||"  |
"."   |
"->"  |
"/"   |
"%"   |
"+"   |
"-"   |
"~"   |
"|"   |
"^"   |
"*"   |
"&"   {
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



	/* Match normal C tokens */



{CKEYWORD} {
	StartToken();
	return ThisToken(kReservedCKeyword);
	}

{CDATATYPE} {
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

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{CCONST} {
	StartToken();
	return ThisToken(kCharConst);
	}

{BADCCONST} {
	StartToken();
	return ThisToken(kBadCharConst);
	}



	/* Match C++ comments */



"///".*\n? {
	StartToken();
	return ThisToken(kDocComment);
	}

"//".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* The following match the C tokens that are too dangerous to eat in one   */
	/* bite because it is so easy to create unterminated comments and strings. */



	/* Match preprocessor directives */



^{WS}("#"|"%:"|"??="){WS}{PPKEYWORD} {
	StartToken();
	SavePPNameRange();
	BEGIN(PP_ARG_STATE);
	}

<PP_ARG_STATE>{

.*\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kPPDirective);
	}

.+  {
	ContinueToken();	/* <<EOF>> is next match */
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kPPDirective);
	}

}



	/* Match C comments -- this is the fast comment scanner from the flex man page,
	   since Vern Paxon presumably can optimize such things far better than I can. */



"/*" {
	StartToken();
	itsIsDocCommentFlag = kJFalse;
	BEGIN(C_COMMENT_STATE);
	}

"/**"/[^/] {
	StartToken();
	itsIsDocCommentFlag = kJTrue;
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
	return ThisToken(itsIsDocCommentFlag ? kDocComment : kComment);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedCComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */



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

\\(.|\n)?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match simple strings. */



"@"\" {
	StartToken();
	BEGIN(SIMPLE_STRING_STATE);
	}

<SIMPLE_STRING_STATE>{

\"\" {
	ContinueToken();
	}

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kString);
	}

.|\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

<<EOF>> {
	BEGIN(INITIAL);
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
