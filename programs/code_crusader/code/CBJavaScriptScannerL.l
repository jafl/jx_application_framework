%{
/*
Copyright (C) 2006 by John Lindal. All rights reserved.

This scanner reads a JavaScript file and returns CBJavaScriptScanner::Tokens.

Remember to upgrade CBHTMLScanner, too!
*/

#define _H_CBJavaScriptScannerL

#include "CBJavaScriptScanner.h"
#include <ctype.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBJavaScriptScanner::Token CBJavaScriptScanner::NextToken()
%}

%option c++ yyclass = "CBJavaScriptScanner" prefix = "CBJavaScript"
%option 8bit nodefault noyywrap
%option full ecs align

%x DOC_COMMENT_STATE C_COMMENT_STATE
%x SINGLE_STRING_STATE DOUBLE_STRING_STATE TEMPLATE_STRING_STATE
%x REGEX_SEARCH_STATE REGEX_CHAR_CLASS_STATE

WS           ([ \v\t\f\r\n]+)



KEYWORD      (abstract|arguments|async|await|boolean|break|byte|case|catch|char|class|const|constructor|continue|debugger|default|delete|do|double|else|enum|eval|export|extends|false|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|let|long|native|new|null|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|true|try|typeof|var|void|volatile|while|with|yield)

DOCTAG       (async|attribute|author|beta|broadcast|bubbles|category|class|chainable|config|constructor|default|deprecated|event|example|extends|extension|extensionfor|extension_for|final|for|main|method|module|namespace|optional|param|private|property|protected|readonly|readOnly|required|requires|return|since|static|submodule|type|uses|writeonce|writeOnce)



IDCAR        ([_[:alpha:]])
IDCDR        ([_[:alnum:]])

ID           ({IDCAR}{IDCDR}*)



DECIMAL      ([0-9]+)
HEX          (0[xX]0*[0-9A-Fa-f]{1,8})
BADHEX       (0[xX][0-9A-Fa-f]{9,})



SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)
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
"!"    |
"*"    |
"%"    |
"<<"   |
">>"   |
">>>"  |
"<"    |
"<="   |
">"    |
">="   |
"=="   |
"==="  |
"!="   |
"!=="  |
"~"    |
"&"    |
"^"    |
"|"    |
"^^"   |
"&&"   |
"||"   |
"?"    |
":"    |
"="    |
"*="   |
"%="   |
"+="   |
"-="   |
"&="   |
"&&="  |
"^="   |
"|="   |
"||="  |
"^^="  |
"<<="  |
">>="  |
">>>=" |
"."    |
"..."  |
"::"   {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kOperator);
	}



	/* Avoid styling JSX tags as regexp */



"</" |
"/>" {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
"[" |
"{" |
"}" |
"," |
";" {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kDelimiter);
	}

")" |
"]" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kDelimiter);
	}



	/* Match normal tokens */



{KEYWORD} {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kReservedKeyword);
	}

{ID} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kID);
	}

{DECIMAL} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kDecimalInt);
	}

{HEX} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kHexInt);
	}

{BADHEX} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBadHexInt);
	}

{FLOAT} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kFloat);
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



\' {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(SINGLE_STRING_STATE);
	}

<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kString);
	}

\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)? |
[^\\\n\']+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

\" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(DOUBLE_STRING_STATE);
	}

<DOUBLE_STRING_STATE>{

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

\` {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(TEMPLATE_STRING_STATE);
	}

<TEMPLATE_STRING_STATE>{

\` {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kTemplateString);
	}

\\.?     |
[^\\\`]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match regex search */



"/"=? {
	if (itsProbableOperatorFlag)
		{
		StartToken();
		itsProbableOperatorFlag = kJFalse;
		return ThisToken(kOperator);
		}
	else
		{
		StartToken();
		itsProbableOperatorFlag = kJTrue;
		BEGIN(REGEX_SEARCH_STATE);
		}
	}

<REGEX_SEARCH_STATE>{

"/"[gim]* {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kRegexSearch);
	}

\[(^-?\])? {
	ContinueToken();
	BEGIN(REGEX_CHAR_CLASS_STATE);
	}

\\.?     |
[^[\\/]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}

<REGEX_CHAR_CLASS_STATE>{

"]" {
	ContinueToken();
	BEGIN(REGEX_SEARCH_STATE);
	}

\\.?    |
[^]\\]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}



	/* Clean up */



{WS} {
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
