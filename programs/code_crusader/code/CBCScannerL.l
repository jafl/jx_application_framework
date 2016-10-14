%{
/*
Copyright (C) 1998 by Dustin Laurence. All rights reserved.

This scanner reads a C/C++ file and returns CBCScanner::Tokens.
*/

#define _H_CBCScannerL

#include "CBCScanner.h"
#include <ctype.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBCScanner::Token CBCScanner::NextToken()
%}

%option c++ yyclass = "CBCScanner" prefix = "CBC"
%option 8bit nodefault noyywrap
%option full ecs align

%x C_COMMENT_STATE STRING_STATE PP_ARG_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



CKEYWORD     (auto|break|case|const|continue|default|do|else|enum|extern|for|goto|if|register|return|sizeof|static|struct|switch|typedef|union|volatile|while)

CPPKEYWORD   (and|and_eq|asm|bitand|bitor|catch|class|compl|const_cast|delete|dynamic_cast|explicit|export|false|final|friend|inline|mutable|namespace|new|not|not_eq|operator|or|or_eq|overload|override|private|protected|public|reinterpret_cast|static_cast|template|this|throw|true|try|typeid|typename|using|virtual|wchar_t|xor|xor_eq)

PPKEYWORD    (include|include_next|define|undef|if|ifdef|ifndef|else|elif|endif|line|pragma|error)

CDATATYPE    (bool|char|double|float|int|long|nullptr|short|signed|unsigned|void)



HEXQUAD      ([0-9a-fA-F]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)



IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})

ID           ({IDCAR}{IDCDR}*)

DOLLARIDCAR  ([_$[:alpha:]]|{UESCCODE})
DOLLARIDCDR  ([_$[:alnum:]]|{UESCCODE})

DOLLARID     ({DOLLARIDCAR}{DOLLARIDCDR}*)



	/* Agrees with Harbison & Steele's BNF */
INTSUFFIX    (([lL][uU]?)|([uU][lL]?))

DECIMAL      ((0|[1-9][0-9]*){INTSUFFIX}?)
HEX          (0[xX][0-9A-Fa-f]*{INTSUFFIX}?)
OCTAL        (0[0-7]+{INTSUFFIX}?)
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
BADINT       ([0-9]+{INTSUFFIX}?)
	/* We can't define BADHEX because 0xAAU is legal while 0xAUA isn't */
	/* and regex subexpressions are greedy. */



	/* Following Harbison & Steele's BNF, except of course I'm using regexes */
FLOATSUFFIX  ([fFlL])
SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}{FLOATSUFFIX}?|{DOTDIGITS}{EXPONENT}?{FLOATSUFFIX}?)



	/* Considerably modified from Harbison & Steele to distinguish ANSI multi-byte  */
	/* numeric escape sequences, since they are non-portable and hazardous.  We can */
	/* warn about ANSI brain-damage with a different color/font. */
CESCCODE     ([abfnrtv\\'"?])
OESCCODE     (0*[1-3]?[0-7]{1,2})
HESCCODE     ([xX]0*[0-9a-fA-F]{1,2})

ESCCHAR      (\\({CESCCODE}|{OESCCODE}|{HESCCODE}))

BADCESCCODE  ([^abfnrtv\\'"?])
BADOESCCODE  ([0-9]+)
BADHESCCODE  ([xX]0*[0-9a-fA-F]{3,})

BADESCCHAR   (\\({BADCESCCODE}|{BADOESCCODE}|{BADHESCCODE}))



	/* Again, I'm treating ANSI multi-byte character constants separately */
CCHAR        ([^\n'\\]|{ESCCHAR}|{UESCCODE})

CCONST       (L?\'{CCHAR}\')
BADCCONST    (L?\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CCHAR}){2,})\')

%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



	/* Match trigraphs and respellings, God help my soul --       */
	/* I could treat them like the symbols they represent instead */



"??"[()<>/=!'-] {
	StartToken();
	return ThisToken(kTrigraph);
	}

"<"[:%] |
"%"[>:] |
":>"    {
	StartToken();
	return ThisToken(kRespelling);
	}



	/* Match misc. legal C/C++ symbols */



\\ { /* Display separately when not part of a string (should add in ID's) */
	StartToken();
	return ThisToken(kContinuation);
	}



	/* Match operators */



"::"  |
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
".*"  |
"->*" |
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



{CKEYWORD} { /* This intentionally doesn't account for ID's which cross newlines! */
	StartToken();
	return ThisToken(kReservedCKeyword);
	}

{CPPKEYWORD} { /* This intentionally doesn't account for ID's which cross newlines! */
	StartToken();
	return ThisToken(kReservedCPPKeyword);
	}

{CDATATYPE} { /* This intentionally doesn't account for ID's which cross newlines! */
	StartToken();
	return ThisToken(kBuiltInDataType);
	}

{ID} { /* This intentionally doesn't account for ID's which cross newlines! */
	StartToken();
	return ThisToken(kID);
	}

{DOLLARID} { /* Treated separately because some systems accept, some don't */
	StartToken();
	return ThisToken(kDollarID);
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

{CCONST} {
	StartToken();
	return ThisToken(kCharConst);
	}

{BADCCONST} {
	StartToken();
	return ThisToken(kBadCharConst);
	}



	/* Match C++ comments */



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

.*\\\n {
	ContinueToken();
	}

\n        |
.*[^\\\n]\n {
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
	return ThisToken(kUnterminatedCComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */



(L|u8?|U)?\" {
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



	/* Clean up */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end or we'd miss CPP directives */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	if ( isprint(YYText()[0]) )
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
