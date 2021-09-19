%top {
/*
Copyright © 2001 by John Lindal.

This scanner reads a Java file and returns CB::Text::Java::Scanner::Tokens.

Remember to upgrade CBHTMLScanner, too!
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
}

%option namespace="CB::Text::Java" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBJavaStyler

	enum TokenType
	{
		kBadHexInt = kEOF+1,
		kBadCharConst,
		kUnterminatedString,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,
		kBuiltInDataType,
		kAnnotation,

		kOperator,
		kDelimiter,

		kString,
		kCharConst,

		kFloat,
		kDecimalInt,
		kHexInt,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBJavaStyler
		};
}

%x DOC_COMMENT_STATE C_COMMENT_STATE STRING_STATE

WS           ([ \v\t\f\r\n]+)



KEYWORD      (abstract|break|byvalue|case|cast|catch|class|ClassLoader|clone|Cloneable|Compiler|const|continue|default|do|enum|else|extends|equals|false|final(ly)?|finalize|for|future|generic|getClass|goto|hashCode|if|implements|import|inner|instanceof|interface|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|package|POSITIVE_INFINITY|private|Process|protected|public|Random|rest|return|Runnable|Runtime|SecurityManager|static|StringTokenizer|super|switch|synchronized|System|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

DATATYPE     (BitSet|[bB]oolean|[bB]yte|char|Character|Class|Date|Dictionary|[dD]ouble|Enumeration|(Runtime)?Exception|[fF]loat|Hashtable|int|Integer|Locale|[lL]ong|Number|Object|Observable|Observer|Properties|[sS]hort|Stack|String(Buffer|Builder)?|Vector)

DOCTAG       (author|code|docRoot|deprecated|exception|inheritDoc|link|linkplain|literal|param|return|see|serial|serialData|serialField|since|throws|value|version)



UESCCODE     (\\[uU][[:xdigit:]]{4})



IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})

ID           ({IDCAR}{IDCDR}*)
ANNOTATION   (@{ID}(\.{ID})*)



INTSUFFIX    ([lL])

DECIMAL      ([0-9]+{INTSUFFIX}?)
HEX          (0[xX]0*[[:xdigit:]]{1,8}{INTSUFFIX}?)
BADHEX       (0[xX][[:xdigit:]]{9,}{INTSUFFIX}?)



FLOATSUFFIX  ([fFlL])
SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}{FLOATSUFFIX}?|{DOTDIGITS}{EXPONENT}?{FLOATSUFFIX}?)



CESCCODE     ([bfnrt\\'"])
OESCCODE     ([1-3]?[0-7]{2})
ESCCHAR      (\\({CESCCODE}|{OESCCODE}))

BADESCCHAR   (\\[^bfnrt\\'"u[:digit:]])



CHAR         ([^\n'\\]|{ESCCHAR}|{UESCCODE})

CCONST       (\'{CHAR}\')
BADCCONST    (\'(\\|{BADESCCHAR}|({BADESCCHAR}|{CHAR}){2,})\'|\'(\\?{CHAR}?))

%%



	/* Match operators */



"++"   |
"--"   |
"+"    |
"-"    |
"~"    |
"!"    |
"*"    |
"/"    |
"%"    |
"<<"   |
">>"   |
">>>"  |
"<"    |
"<="   |
">"    |
">="   |
"=="   |
"!="   |
"&"    |
"^"    |
"|"    |
"&&"   |
"||"   |
"?"    |
":"    |
"="    |
"*="   |
"/="   |
"%="   |
"+="   |
"-="   |
"<<="  |
">>="  |
">>>=" |
"&="   |
"^="   |
"|="   |
"."    {
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

{ID} {
	StartToken();
	return ThisToken(kID);
	}

{ANNOTATION} {
	StartToken();
	return ThisToken(kAnnotation);
	}

{DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{BADHEX} {
	StartToken();
	return ThisToken(kBadHexInt);
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



	/* The following match tokens that are too dangerous to eat in one bite */
	/* because it is so easy to create unterminated comments and strings.   */



	/* Match DOC comments -- this is based on the fast comment scanner from the flex man */
	/* page, since Vern Paxon presumably can optimize such things far better than I can. */



"/**/" {
	StartToken();
	return ThisToken(kComment);
	}

"/**"/[^/] {
	StartToken();
	start(DOC_COMMENT_STATE);
	}

<DOC_COMMENT_STATE>{

[^*<@\n]*\n?      |
"*"+[^*/<@\n]*\n? |
[<@]              {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kComment);
	}

"<"[^*>]+> {		/* comment ends with <star><slash> even if in the middle of <...> */
	return DocToken(kDocCommentHTMLTag);
	}

@{DOCTAG} {
	return DocToken(kDocCommentSpecialTag);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match C comments -- this is the fast comment scanner from the flex man page, */
	/* since Vern Paxon presumably can optimize such things far better than I can.  */



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



	/* Match strings -- based on the string scanner from the flex manual. */
	/* Java does not support \\\n to continue a string on the next line.  */



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

\\.?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
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
