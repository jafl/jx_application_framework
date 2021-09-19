%top {
/*
Copyright © 1998 by Dustin Laurence.

This scanner reads a C/C++ file and returns CB::Text::C::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
}

%option namespace="CB::Text::C" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBCStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
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
}

%x C_COMMENT_STATE STRING_STATE PP_ARG_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



CKEYWORD     (auto|break|case|const|continue|default|do|else|enum|extern|for|goto|if|inline|register|restrict|return|sizeof|static|struct|switch|typedef|union|volatile|while)

CPPKEYWORD   (alignas|aslignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|bitand|bitor|catch|class|compl|concept|constexpr|const_cast|decltype|delete|dynamic_cast|explicit|export|false|final|friend|module|mutable|namespace|new|noexcept|not|not_eq|operator|or|or_eq|overload|override|private|protected|public|reinterpret_cast|requires|static_assert|static_cast|template|this|thread_local|throw|true|try|typeid|typename|using|virtual|xor|xor_eq)

PPKEYWORD    (include|include_next|define|undef|if|ifdef|ifndef|else|elif|endif|line|pragma|error)

CDATATYPE    (bool|char|char16_t|char32_t|double|float|int|long|nullptr|short|signed|unsigned|void|wchar_t)



HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)



IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})

ID           ({IDCAR}{IDCDR}*)

DOLLARIDCAR  (_|\$|\p{L}|{UESCCODE}|{UESCCODE})
DOLLARIDCDR  (_|\$|\p{L}|\d|{UESCCODE}|{UESCCODE})

DOLLARID     ({DOLLARIDCAR}{DOLLARIDCDR}*)


%{
	/* Agrees with Harbison & Steele's BNF */
%}
INTSUFFIX    (([lL][uU]?)|([uU][lL]?))

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
FLOATSUFFIX  ([fFlL])
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
CESCCODE     ([abfnrtv\\'"?])
OESCCODE     (0*[1-3]?[0-7]{1,2})
HESCCODE     ([xX]0*[[:xdigit:]]{1,2})

ESCCHAR      (\\({CESCCODE}|{OESCCODE}|{HESCCODE}))

BADCESCCODE  ([^abfnrtv\\'"?])
BADOESCCODE  ([0-9]+)
BADHESCCODE  ([xX]0*[[:xdigit:]]{3,})

BADESCCHAR   (\\({BADCESCCODE}|{BADOESCCODE}|{BADHESCCODE}))


%{
	/* Again, I'm treating ANSI multi-byte character constants separately */
%}
CCHAR        ([^\n'\\]|{ESCCHAR}|{UESCCODE})

CCONST       (L?\'{CCHAR}\')
BADCCONST    (L?\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CCHAR}){2,})\'|L?\'(\\?{CCHAR}?))

%%



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



	/* Match preprocessor directives - digraphs removed from C++17 */



^{WS}"#"{WS}{PPKEYWORD} {
	StartToken();
	SavePPNameRange();
	start(PP_ARG_STATE);
	}

<PP_ARG_STATE>{

.*\\\n {
	ContinueToken();
	}

\n        |
.*[^\\\n]\n {
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
	return ThisToken(kComment);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedCComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */



(L|u8?|U)?\" {
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
