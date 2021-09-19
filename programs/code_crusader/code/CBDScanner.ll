%top {
/*
Copyright © 2021 by John Lindal.
Copyright © 1998 by Dustin Laurence.

This scanner reads a D file and returns CB::Text::D::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::D" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBDStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
		kBadCharConst,
		kUnterminatedString,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,

		kOperator,
		kDelimiter,

		kString,
		kWysiwygString,
		kHexString,
		kTokenString,
		kCharConst,

		kFloat,
		kHexFloat,
		kDecimalInt,
		kBinaryInt,
		kHexInt,

		kComment,

		kError,			// place holder for CBDStyler

		// special cases

		kContinuation
	};

private:

	JSize	itsDepth;
}

%x C_COMMENT_STATE D_COMMENT_STATE
%x STRING_STATE WYSIWYG_STRING_STATE ALT_WYSIWYG_STRING_STATE
%x HEX_STRING_STATE TOKEN_STRING_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD     (abstract|alias|align|asm|assert|auto|body|break|case|cast|catch|class|const|continue|debug|default|delegate|delete|deprecated|do|else|enum|export|extern|false|final|finally|for|foreach|foreach_reverse|function|goto|if|immutable|import|in|inout|interface|invariant|is|lazy|long|macro|mixin|module|new|nothrow|null|out|override|package|pragma|private|protected|public|pure|ref|return|scope|shared|static|struct|super|switch|synchronized|template|this|throw|true|try|typeid|typeof|union|unittest|version|void|while|with|__DATE__|__EOF__|__FILE__|__FILE_FULL_PATH__|__FUNCTION__|__LINE__|__MODULE__|__PRETTY_FUNCTION__|__TIME__|__TIMESTAMP__|__VENDOR__|__VERSION__|__gshared|__traits|__vector|__parameters)

DATATYPE    (bool|byte|cdouble|cent|cfloat|char|creal|dchar|double|float|idouble|ifloat|int|ireal|real|short|ubyte|ucent|uint|ulong|ushort|wchar)



IDCAR        (_|\p{L})
IDCDR        (_|\p{L}|\d)

ID           ({IDCAR}{IDCDR}*)



INTSUFFIX    ((L[uU]?)|([uU]L?))

DECIMAL      ((0|[1-9]+(_?[0-9]+)*){INTSUFFIX}?)
BINARY       (0[bB][0-1]+(_?[0-1]+)*{INTSUFFIX}?)
HEX          (0[xX][[:xdigit:]]+(_?[[:xdigit:]]+)*{INTSUFFIX}?)
%{
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
%}
BADDECIMAL   ([0-9]+{INTSUFFIX}?)
BADBINARY    (0[bB][0-9]+{INTSUFFIX}?)
%{
	/* We can't define BADHEX because 0xAAU is legal while 0xAUA isn't */
	/* and regex subexpressions are greedy. */
%}


FLOATSUFFIX  ([fFL]i?)
DIGITSEQ     ([0-9]+(_?[0-9]+)*)
EXPONENT     ([eE][+-]?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}{FLOATSUFFIX}?|{DOTDIGITS}{EXPONENT}?{FLOATSUFFIX}?)



HEXDIGITSEQ  ([[:xdigit:]]+(_?[[:xdigit:]]+)*)
HEXEXPONENT  ([pP][+-]?{DIGITSEQ})

HEXFLOAT     (0[xX]({HEXDIGITSEQ}|{HEXDIGITSEQ}\.{HEXDIGITSEQ}|\.{HEXDIGITSEQ}){HEXEXPONENT}{FLOATSUFFIX}?)



DESCCODE     ([abfnrtv\\'"?])
OESCCODE     ([0-7]{1,3})
HESCCODE     (x[[:xdigit:]]{2})
UESCCODE     (u[[:xdigit:]]{4}|U[[:xdigit:]]{8})
ESCCHAR      (\\({DESCCODE}|{OESCCODE}|{HESCCODE}|{UESCCODE}))

BADDESCCODE  ([^abfnrtv\\'"?])
BADOESCCODE  ([0-9]+)
BADHESCCODE  (x[[:xdigit:]]{3,})
BADUESCCODE  (u[[:xdigit:]]{1,3}|u[[:xdigit:]]{5,}|U[[:xdigit:]]{1,7}|U[[:xdigit:]]{9,})
BADESCCHAR   (\\({BADDESCCODE}|{BADOESCCODE}|{BADHESCCODE}|{BADUESCCODE}))



CCHAR        ([^\n'\\]|{ESCCHAR}|\\&[^;]+;)

CCONST       (\'{CCHAR}\')
BADCCONST    (\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CCHAR}){2,})\'|\'(\\?{CCHAR}?))

%%



	/* Match misc. legal D symbols */



\\ { /* Display separately when not part of a string (should add in ID's) */
	StartToken();
	return ThisToken(kContinuation);
	}



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
">>>=" |
"&="  |
"^="  |
"^^=" |
"|="  |
"~="  |
"<<"  |
">>"  |
">>>" |
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
".."  |
"..." |
"->"  |
"=>"  |
"/"   |
"%"   |
"+"   |
"-"   |
"~"   |
"|"   |
"^"   |
"^^"  |
"*"   |
"&"   |
"@"   |
"#"   |
"$"   {
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



	/* Match normal D tokens */



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

{BINARY} {
	StartToken();
	return ThisToken(kBinaryInt);
	}

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{BADDECIMAL} |
{BADBINARY}  {
	StartToken();
	return ThisToken(kBadInt);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{HEXFLOAT} {
	StartToken();
	return ThisToken(kHexFloat);
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
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match D comments */



"/+" {
	StartToken();
	itsDepth = 1;
	start(D_COMMENT_STATE);
	}

<D_COMMENT_STATE>{

[^+/\n]*\n?     |
"+"+[^+/\n]*\n? |
"/"				{
	ContinueToken();
	}

"/+" {
	ContinueToken();
	itsDepth++;
	}

"+"+"/" {
	ContinueToken();
	itsDepth--;
	if (itsDepth == 0)
		{
		start(INITIAL);
		return ThisToken(kComment);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */



\" {
	StartToken();
	start(STRING_STATE);
	}

<STRING_STATE>{

\"[cwd]? {
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



[rq]\" {
	StartToken();
	start(WYSIWYG_STRING_STATE);
	}

<WYSIWYG_STRING_STATE>{

\"[cwd]? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kWysiwygString);
	}

[^\"]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



\` {
	StartToken();
	start(ALT_WYSIWYG_STRING_STATE);
	}

<ALT_WYSIWYG_STRING_STATE>{

\`[cwd]? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kWysiwygString);
	}

[^\`]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



x\" {
	StartToken();
	start(HEX_STRING_STATE);
	}

<HEX_STRING_STATE>{

\"[cwd]? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kHexString);
	}

[[:xdigit:][:space:]]+ {
	ContinueToken();
	}

[^"] {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



"q{" {
	StartToken();
	itsDepth = 1;
	start(TOKEN_STRING_STATE);
	}

<TOKEN_STRING_STATE>{

"}" {
	ContinueToken();
	itsDepth--;
	if (itsDepth == 0)
		{
		start(INITIAL);
		return ThisToken(kTokenString);
		}
	}

[^}]+ {
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
