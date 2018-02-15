%{
/*
Copyright (C) 2013 by John Lindal.

This scanner reads a PHP .ini file and returns CBINIScanner::Tokens.
*/

#define _H_CBINIScannerL

#include "CBINIScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBINIScanner::Token CBINIScanner::NextToken()
%}

%option c++ yyclass = "CBINIScanner" prefix = "CBINI"
%option 8bit nodefault noyywrap
%option full ecs align

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)

%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



	/* Match normal tokens */



^"["[^]]+"]" {
	StartToken();
	return ThisToken(kSectionName);
	}

"["[^]]*"]" {
	StartToken();
	return ThisToken(kArrayIndex);
	}

[^['";= \v\t\f\r]+ {
	StartToken();
	return ThisToken(kKeyOrValue);
	}

= {
	StartToken();
	return ThisToken(kAssignment);
	}



	/* Match comments */



^{WS}";".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* Match single quoted strings */



\' {
	StartToken();
	BEGIN(SINGLE_STRING_STATE);
	}

<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\\(.|\n)? |
[^\\\']+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match double quoted strings */



\" {
	StartToken();
	BEGIN(DOUBLE_STRING_STATE);
	}

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



	/* Miscellaneous */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end or we'd miss PP directives */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	return ThisToken(kNonPrintChar);
	}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
