%{
/*
Copyright © 2001 by John Lindal. All rights reserved.

This scanner reads a bash file and returns CBCShellScanner::Tokens.
*/

#define _H_CBCShellScannerL

#include "CBCShellScanner.h"
#include <ctype.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBCShellScanner::Token CBCShellScanner::NextToken()
%}

%option c++ yyclass = "CBCShellScanner" prefix = "CBCShell"
%option 8bit nodefault noyywrap
%option full ecs align

%x ID_STATE VAR_STATE
%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE

WSCHAR         ([ \v\t\f\r\n]|\\\n)
WS             ({WSCHAR}+)

VARNAME        ([?#%]?[[:alpha:]_][[:alnum:]_]{0,19}|%?[0-9]+)

RESERVEDWORD   (break|breaksw|case|continue|default|else|end|endif|endsw|foreach|if|then|repeat|switch|while)
BUILTINCMD     (true|false|alias|alloc|bg|bindkey|builtins|bye|cd|chdir|complete|dirs|echo|echotc|eval|exec|exit|fg|filetest|getspath|getxvers|glob|goto|hashstat|history|hup|inlib|jobs|kill|limit|log|login|logout|migrate|newgrp|nice|nohup|notify|onintr|popd|printenv|pushd|rehash|rootnode|sched|set|setenv|setpath|setspath|settc|setty|setxvers|shift|source|stop|suspend|telltc|time|umask|unalias|uncomplete|unhash|universe|unlimit|unset|unsetenv|ver|wait|warp|watchlog|where|which)
%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}

"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}

\' {
	StartToken();
	BEGIN(SINGLE_STRING_STATE);
	}

\" {
	StartToken();
	BEGIN(DOUBLE_STRING_STATE);
	}

\` {
	StartToken();
	BEGIN(EXEC_STRING_STATE);
	}

"$" {
	StartToken();
	BEGIN(VAR_STATE);
	}

{RESERVEDWORD} {
	StartToken();
	return ThisToken(kReservedWord);
	}

{BUILTINCMD} {
	StartToken();
	return ThisToken(kBuiltInCommand);
	}

[^-&|;<>()!^=+*/%~#'"`$\\ \v\t\f\r\n]+ |
\\.?                           {
	StartToken();
	BEGIN(ID_STATE);
	}

"&"  |
"|"  |
";"  |
"("  |
")"  |
"&&" |
"||" {
	StartToken();
	return ThisToken(kControlOperator);
	}

"<"    |
"<<"   |
">"    |
">!"   |
">&"   |
">&!"  |
">>"   |
">>!"  |
">>&"  |
">>&!" {
	StartToken();
	return ThisToken(kRedirectionOperator);
	}

!  |
!! |
!? |
!# {
	StartToken();
	return ThisToken(kHistoryOperator);
	}

!({WSCHAR}|\() {
	StartToken();
	itsCurrentRange.last = itsCurrentRange.first;
	yyless(yyleng-1);
	return ThisToken(kControlOperator);
	}

"="  |
"^"  |
"==" |
"!=" |
"=~" |
"!~" |
"<=" |
">=" |
"+"  |
"++" |
"-"  |
"--" |
"*"  |
"/"  |
"%"  |
"~"  {
	StartToken();
	return ThisToken(kMathOperator);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

	/* Match word */



<ID_STATE>{

[^-&|;<>()!^=+*/%~#'"`$\\ \v\t\f\r\n]+ |
\\.?                                   {
	ContinueToken();
	}

.|{WS} {
	yyless(0);
	BEGIN(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match variable */



<VAR_STATE>{

{VARNAME}        |
"{"{VARNAME}[}[] |
[*#?$!_<]        {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kVariable);
	}

"{"  |
"{}" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kEmptyVariable);
	}

.|\n {
	yyless(0);
	BEGIN(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match strings */



<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?  |
[^\\\n\']+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kDoubleQuoteString);
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

<EXEC_STRING_STATE>{

\` {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kExecString);
	}

\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?  |
[^\\\n\`]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
