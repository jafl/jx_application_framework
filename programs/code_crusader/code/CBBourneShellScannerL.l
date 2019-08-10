%{
/*
Copyright (C) 2001 by John Lindal.

This scanner reads a bash file and returns CBBourneShellScanner::Tokens.
*/

#define _H_CBBourneShellScannerL

#include "CBBourneShellScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBBourneShellScanner::Token CBBourneShellScanner::NextToken()
%}

%option c++ yyclass = "CBBourneShellScanner" prefix = "CBBourneShell"
%option 8bit nodefault noyywrap
%option full ecs align

%x ID_STATE VAR_STATE
%x DOUBLE_STRING_STATE EXEC_STRING_STATE

WSCHAR         ([ \v\t\f\r\n]|\\\n)
WS             ({WSCHAR}+)

VARNAME        ([[:alpha:]_][[:alnum:]_]*|[0-9]+)

RESERVEDWORD   (case|do|done|elif|else|esac|fi|for|function|if|in|select|then|until|while)
BUILTINCMD     (true|false|source|alias|bg|bind|break|builtin|cd|command|continue|declare|typeset|dirs|echo|enable|eval|exec|exit|export|fc|fg|getopts|hash|help|history|jobs|kill|let|local|logout|popd|pushd|pwd|read|readonly|return|set|shift|suspend|test|times|trap|type|ulimit|umask|unalias|unset|wait)
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

\'[^']*\'? {
	StartToken();
	return ThisToken(yyleng > 1 && yytext[yyleng-1] == '\'' ?
					 kSingleQuoteString : kUnterminatedString);
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

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
	StartToken();
	BEGIN(ID_STATE);
	}

"||" |
"&"  |
"&&" |
";"  |
";;" |
"("  |
")"  |
"|"  {
	StartToken();
	return ThisToken(kControlOperator);
	}

"<"   |
"<<"  |
"<<-" |
"<&"  |
">"   |
">|"  |
">>"  |
"&>"  |
">&"  |
"<>"  {
	StartToken();
	return ThisToken(kRedirectionOperator);
	}

!    |
!!   |
!"?" |
!#   {
	StartToken();
	return ThisToken(kHistoryOperator);
	}

!({WSCHAR}|[=(]) {
	StartToken();
	itsCurrentRange.charRange.last = itsCurrentRange.charRange.first;
	itsCurrentRange.byteRange.last = itsCurrentRange.byteRange.first;	// single byte character
	yyless(yyleng-1);
	return ThisToken(kControlOperator);
	}

= {
	StartToken();
	return ThisToken(kOtherOperator);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

	/* Match word */



<ID_STATE>{

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
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

{VARNAME}                                                  |
"{"[#!]?{VARNAME}("["[^]\n]+"]")?("}"|:[-=?+]?|"#"{1,2}|"%"{1,2}|"/"{1,2}) |
[-0-9*@#?$!_]                                              {
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



<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kDoubleQuoteString);
	}

\\(.|\n)? |
[^\\\"]+  {
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

\\(.|\n)? |
[^\\\`]+   {
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
