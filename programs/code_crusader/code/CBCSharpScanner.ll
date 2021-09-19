%top {
/*
Copyright © 2004 by John Lindal.

This scanner reads a C# file and returns CB::Text::CSharp::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
}

%option namespace="CB::Text::CSharp" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBCSharpStyler

	enum TokenType
	{
		kBadCharConst = kEOF+1,
		kUnterminatedString,
		kUnterminatedCComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedCKeyword,
		kBuiltInDataType,

		kOperator,
		kDelimiter,

		kString,
		kCharConst,

		kFloat,
		kDecimalInt,
		kHexInt,

		kComment,
		kDocComment,
		kPPDirective,

		kError			// place holder for CBCSharpStyler
	};

private:

	bool	itsIsDocCommentFlag;
}

%x C_COMMENT_STATE STRING_STATE SIMPLE_STRING_STATE PP_ARG_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



CKEYWORD     (abstract|as|base|break|case|catch|checked|class|const|continue|default|delegate|do|else|enum|explicit|extern|false|finally|for|foreach|goto|if|implicit|in|interface|internal|is|lock|namespace|new|null|operator|out|override|params|private|protected|public|readonly|ref|return|sealed|sizeof|stackalloc|static|struct|switch|this|throw|true|try|typeof|unchecked|unsafe|using|virtual|volatile|while)

PPKEYWORD    (if|else|elif|endif|define|undef|warning|error|line|region|endregion)

CDATATYPE    (bool|byte|char|decimal|double|event|fixed|float|int|long|object|sbyte|short|string|uint|ulong|ushort|void)



HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)



IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})

ID           @?({IDCAR}{IDCDR}*)


%{
	/* Agrees with Harbison & Steele's BNF */
%}
INTSUFFIX    (([lL][uU]?)|([uU][lL]?))

DECIMAL      ([0-9]+{INTSUFFIX}?)
HEX          (0[xX][[:xdigit:]]+{INTSUFFIX}?)
%{
	/* We can't define BADHEX because 0xAAU is legal while 0xAUA isn't */
	/* and regex subexpressions are greedy. */



	/* Following Harbison & Steele's BNF, except of course I'm using regexes */
%}
FLOATSUFFIX  ([fFdDmM])
SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}{FLOATSUFFIX}?|{DOTDIGITS}{EXPONENT}?{FLOATSUFFIX}?)


%{
	/* Considerably modified from Harbison & Steele to distinguish ANSI multi-byte  */
	/* numeric escape sequences, since they are non-portable and hazardous.  We can */
	/* warn about ANSI brain-damage with a different color/font. */
%}
CESCCODE     ([abfnrtv\\'"])
HESCCODE     ([xX]0*[[:xdigit:]]{1,4})

ESCCHAR      (\\({CESCCODE}|{HESCCODE}))

BADCESCCODE  ([^abfnrtv\\'"])
BADHESCCODE  ([xX]0*[[:xdigit:]]{5,})

BADESCCHAR   (\\({BADCESCCODE}|{BADHESCCODE}))


%{
	/* Again, I'm treating ANSI multi-byte character constants separately */
%}
CCHAR        ([^\n'\\]|{ESCCHAR}|{UESCCODE})

CCONST       (\'{CCHAR}\')
BADCCONST    (\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CCHAR}){2,})\')

%%



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



^{WS}"#"{WS}{PPKEYWORD} {
	StartToken();
	SavePPNameRange();
	start(PP_ARG_STATE);
	}

<PP_ARG_STATE>{

.*\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kPPDirective);
	}

.+  {
	ContinueToken();	/* <<EOF>> is next match */
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kPPDirective);
	}

}



	/* Match C comments -- this is the fast comment scanner from the flex man page,
	   since Vern Paxon presumably can optimize such things far better than I can. */



"/*" {
	StartToken();
	itsIsDocCommentFlag = false;
	start(C_COMMENT_STATE);
	}

"/**"/[^/] {
	StartToken();
	itsIsDocCommentFlag = true;
	start(C_COMMENT_STATE);
	}

<C_COMMENT_STATE>{

[^*\n]*\n?      |
"*"+[^*/\n]*\n? {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(itsIsDocCommentFlag ? kDocComment : kComment);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedCComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */



\" {
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

\\(.|\n)?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match simple strings. */



"@"\" {
	StartToken();
	start(SIMPLE_STRING_STATE);
	}

<SIMPLE_STRING_STATE>{

\"\" {
	ContinueToken();
	}

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kString);
	}

.|\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
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
