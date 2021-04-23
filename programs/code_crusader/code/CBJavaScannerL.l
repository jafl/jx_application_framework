%{
/*
Copyright (C) 2001 by John Lindal.

This scanner reads a Java file and returns CBJavaScanner::Tokens.

Remember to upgrade CBHTMLScanner, too!
*/

#define _H_CBJavaScannerL

#include "CBJavaScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBJavaScanner::Token CBJavaScanner::NextToken()
%}

%option c++ yyclass = "CBJavaScanner" prefix = "CBJava"
%option 8bit nodefault noyywrap
%option full ecs align

%x DOC_COMMENT_STATE C_COMMENT_STATE STRING_STATE

WS           ([ \v\t\f\r\n]+)



KEYWORD      (abstract|break|byvalue|case|cast|catch|class|ClassLoader|clone|Cloneable|Compiler|const|continue|default|do|enum|else|extends|equals|false|final(ly)?|finalize|for|future|generic|getClass|goto|hashCode|if|implements|import|inner|instanceof|interface|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|package|POSITIVE_INFINITY|private|Process|protected|public|Random|rest|return|Runnable|Runtime|SecurityManager|static|StringTokenizer|super|switch|synchronized|System|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

DATATYPE     (BitSet|[bB]oolean|[bB]yte|char|Character|Class|Date|Dictionary|[dD]ouble|Enumeration|(Runtime)?Exception|[fF]loat|Hashtable|int|Integer|Locale|[lL]ong|Number|Object|Observable|Observer|Properties|[sS]hort|Stack|String(Buffer|Builder)?|Vector)

DOCTAG       (author|code|docRoot|deprecated|exception|inheritDoc|link|linkplain|literal|param|return|see|serial|serialData|serialField|since|throws|value|version)



UESCCODE     (\\[uU][0-9a-fA-F]{4})



IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})

ID           ({IDCAR}{IDCDR}*)
ANNOTATION   (@{ID}(\.{ID})*)



INTSUFFIX    ([lL])

DECIMAL      ([0-9]+{INTSUFFIX}?)
HEX          (0[xX]0*[0-9A-Fa-f]{1,8}{INTSUFFIX}?)
BADHEX       (0[xX][0-9A-Fa-f]{9,}{INTSUFFIX}?)



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

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



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

	/* remember to update CBJavaScanner */



"/**/" {
	StartToken();
	return ThisToken(kComment);
	}

"/**"/[^/] {
	StartToken();
	BEGIN(DOC_COMMENT_STATE);
	}

<DOC_COMMENT_STATE>{

[^*<@\n]*\n?      |
"*"+[^*/<@\n]*\n? |
[<@]              {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kComment);
	}

\<[^*>]+> {		/* comment ends with <star><slash> even if in the middle of <...> */
	return DocToken(kDocCommentHTMLTag);
	}

@{DOCTAG} {
	return DocToken(kDocCommentSpecialTag);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



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
	/* Java does not support \\\n to continue a string on the next line.  */



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
