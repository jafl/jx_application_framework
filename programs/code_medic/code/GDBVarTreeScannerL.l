%{
/*
Copyright (C) 2001 by John Lindal.

This scanner reads output from gdb's print command.
*/

#define _H_GDBVarTreeScannerL

#include "GDBVarTreeScanner.h"
#include <JStringIterator.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL int GDBVarTreeScanner::yylex(YYSTYPE* lvalp)

#define NEW_TOKEN(s) \
	lvalp->pString = jnew JString(s); \
	assert( lvalp->pString != nullptr );
%}

%option c++ yyclass = "GDBVarTreeScanner" prefix="GDBVar"
%option 8bit nodefault stack
%option noyywrap

%x DOUBLE_QUOTE_STATE SINGLE_QUOTE_STATE BRACKET_STATE PAREN_STATE
%x TEMPLATE_NAME_STATE TEMPLATE_NAME_BRACKET_STATE

WS			([[:space:]]*)

INT			([[:digit:]]+)
HEX			(0x[[:xdigit:]]+)
FLOAT		({INT}\.|{INT}?\.{INT})
EXP			(({INT}|{FLOAT})e[+-]?{INT})
INF			([iI][nN][fF])
NAN			([nN][aA][nN]\({HEX}\))

ID			([[:alpha:]$_][[:alnum:]$_.]*)
NAME		({ID}(::{ID})*)

STDCLASS	(std::[a-z]+)
STDWRAP		({STDCLASS}" wrapping: ")

%%

%{
/************************************************************************/

	bzero(lvalp, sizeof(YYSTYPE));
%}

[+-]?{INT} {
	NEW_TOKEN(yytext);
	return P_INTEGER;
	}

[+-]?{INT}/\.\.\. {
	NEW_TOKEN(yytext);
	return P_INTEGER;
	}

[+-]?{FLOAT} |
[+-]?{EXP}   |
[+-]?{INF}   |
[+-]?{NAN}   {
	NEW_TOKEN(yytext);
	return P_FLOAT;
	}

{HEX} {
	NEW_TOKEN(yytext);
	return P_HEX;
	}

"{" {
	return P_GROUP_OPEN;
	}

"}" {
	return P_GROUP_CLOSE;
	}

	/* "static" must come before identifier to be separate rule */

static {
	NEW_TOKEN(yytext);
	return P_STATIC;
	}

{NAME} {
	NEW_TOKEN(yytext);
	return P_NAME;
	}

{NAME}/"<" {
	itsString = yytext;
	BEGIN(TEMPLATE_NAME_STATE);
	}

{STDWRAP}*{STDCLASS}" of length 0, capacity "[+-]?{INT} |
{STDWRAP}*{STDCLASS}" with 0 elements"                  |
{STDWRAP}*"empty "{STDCLASS}                            {
	NEW_TOKEN(yytext);
	return P_EMPTY_SUMMARY;
	}

{STDWRAP}*{STDCLASS}" of length "[+-]?{INT}", capacity "[+-]?{INT}{WS}= |
{STDWRAP}*{STDCLASS}" with "[+-]?{INT}" elements"{WS}=                  |
{STDWRAP}*{STDCLASS}{WS}=                                               {
	NEW_TOKEN(yytext);
	JStringIterator iter(lvalp->pString, kJIteratorStartAtEnd);
	iter.RemovePrev();
	return P_SUMMARY;
	}

{NAME}{WS}=    |
\[{INT}\]{WS}= {
	NEW_TOKEN(yytext);
	return P_NAME_EQ;
	}

<TEMPLATE_NAME_STATE>{

\< {
	itsString      += "<";
	itsBracketCount = 1;
	yy_push_state(TEMPLATE_NAME_BRACKET_STATE);
	}

::{ID} {
	itsString += yytext;
	}

[^<] {
	yyless(0);
	BEGIN(INITIAL);
	NEW_TOKEN(itsString);
	return P_NAME;
}

}

<TEMPLATE_NAME_BRACKET_STATE>{

\< {
	itsBracketCount++;
	itsString += "<";
	}

[^<>]+ {
	itsString += yytext;
	}

\> {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		yy_pop_state();
		}
	}

}



\" {
	itsString = "\"";
	yy_push_state(DOUBLE_QUOTE_STATE);
	}

<DOUBLE_QUOTE_STATE>{

\\.?     |
[^\\\"]+ {
	itsString += yytext;
	}

\"(\.\.\.)?(", <incomplete sequence "[^>]+">")? {
	itsString += yytext;
	yy_pop_state();
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		return P_STRING;
		}
	}
}



\' {
	itsString = "\'";
	BEGIN(SINGLE_QUOTE_STATE);
	}

<SINGLE_QUOTE_STATE>{

\\.?     |
[^\\\']+ {
	itsString += yytext;
	}

\' {
	itsString += "\'";
	NEW_TOKEN(itsString);
	BEGIN(INITIAL);
	return P_CHAR;
	}
}



"<>" {
	NEW_TOKEN(yytext);
	return P_EMPTY_BRACKET;
	}

"<>"{WS}= {
	NEW_TOKEN(yytext);
	return P_EMPTY_BRACKET_EQ;
	}

"<No data fields>" {
	NEW_TOKEN(yytext);
	return P_NO_DATA_FIELDS;
	}

\< {
	itsString       = "<";
	itsBracketCount = 1;
	BEGIN(BRACKET_STATE);
	}

<BRACKET_STATE>{

\< {
	itsBracketCount++;
	itsString += "<";
	}

\\.?       |
[^<>\"\\]+ {
	itsString += yytext;
	}

\" {
	itsString += "\"";
	yy_push_state(DOUBLE_QUOTE_STATE);
	}

\> {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		BEGIN(INITIAL);
		return P_BRACKET;
		}
	}

\>{WS}= {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		BEGIN(INITIAL);
		return P_BRACKET_EQ;
		}
	}
}



\( {
	itsString       = "(";
	itsBracketCount = 1;
	BEGIN(PAREN_STATE);
	}

<PAREN_STATE>{

\( {
	itsBracketCount++;
	itsString += "(";
	}

\\.?       |
[^()\"\\]+ {
	itsString += yytext;
	}

\" {
	itsString += "\"";
	yy_push_state(DOUBLE_QUOTE_STATE);
	}

\) {
	itsBracketCount--;
	itsString += ")";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		BEGIN(INITIAL);
		return P_PAREN_EXPR;
		}
	}
}



[[:space:]]+	/* ignore */

	/* bison allows us to return single characters */

. return yytext[0];

<<EOF>> {
	itsBracketCount = 0;
	return P_EOF;
	}

%%
