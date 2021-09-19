%top {
/*
Copyright (C) 2001 by John Lindal.

This scanner reads output from gdb's print command.
*/

#include "CMVarNode.h"
#include "GDBVarGroupInfo.h"
#include <JString.h>
#include <JStringIterator.h>
#include "GDBVarTreeParserY.hpp"
#include <jAssert.h>

#define NEW_TOKEN(s) \
	lvalp->pString = jnew JString(s); \
	assert( lvalp->pString != nullptr );
%}

%option namespace="GDB::VarTree" lexer="Scanner"
%option lex="NextToken" params="YYSTYPE* lvalp"
%option unicode nodefault full freespace
%option bison noyywrap

%class {

private:

	JString	itsString;
	JSize	itsBracketCount;
}

%init {
	itsBracketCount = 0;
}

%x DOUBLE_QUOTE_STATE SINGLE_QUOTE_STATE BRACKET_STATE PAREN_STATE
%x TEMPLATE_NAME_STATE TEMPLATE_NAME_BRACKET_STATE

WS			([[:space:]]*)

INT			([[:digit:]]+)
HEX			(0x[[:xdigit:]]+)
FLOAT		({INT}\.|{INT}?\.{INT})
EXP			(({INT}|{FLOAT})e[+-]?{INT})
INF			([iI][nN][fF])
NAN			([nN][aA][nN]\({HEX}\))

ID			((_|\$|\p{L})(_|\$|\.|\p{L}|\d)*)
NAME		({ID}(::{ID})*)

STDCLASS	(std::[a-z]+)
STDWRAP		({STDCLASS}" wrapping: ")

%%

%{
/************************************************************************/

	bzero(lvalp, sizeof(YYSTYPE));
%}

[+-]?{INT} {
	NEW_TOKEN(text());
	return P_INTEGER;
	}

[+-]?{INT}/\.\.\. {
	NEW_TOKEN(text());
	return P_INTEGER;
	}

[+-]?{FLOAT} |
[+-]?{EXP}   |
[+-]?{INF}   |
[+-]?{NAN}   {
	NEW_TOKEN(text());
	return P_FLOAT;
	}

{HEX} {
	NEW_TOKEN(text());
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
	NEW_TOKEN(text());
	return P_STATIC;
	}

{NAME} {
	NEW_TOKEN(text());
	return P_NAME;
	}

{NAME}/"<" {
	itsString = text();
	start(TEMPLATE_NAME_STATE);
	}

{STDWRAP}*{STDCLASS}" of length 0, capacity "[+-]?{INT} |
{STDWRAP}*{STDCLASS}" with 0 elements"                  |
{STDWRAP}*"empty "{STDCLASS}                            {
	NEW_TOKEN(text());
	return P_EMPTY_SUMMARY;
	}

{STDWRAP}*{STDCLASS}" of length "[+-]?{INT}", capacity "[+-]?{INT}{WS}= |
{STDWRAP}*{STDCLASS}" with "[+-]?{INT}" elements"{WS}=                  |
{STDWRAP}*{STDCLASS}{WS}=                                               {
	NEW_TOKEN(text());
	JStringIterator iter(lvalp->pString, kJIteratorStartAtEnd);
	iter.RemovePrev();
	return P_SUMMARY;
	}

{NAME}{WS}=    |
\[{INT}\]{WS}= {
	NEW_TOKEN(text());
	return P_NAME_EQ;
	}

<TEMPLATE_NAME_STATE>{

"<" {
	itsString      += "<";
	itsBracketCount = 1;
	push_state(TEMPLATE_NAME_BRACKET_STATE);
	}

::{ID} {
	itsString += text();
	}

[^<] {
	matcher().less(0);
	start(INITIAL);
	NEW_TOKEN(itsString);
	return P_NAME;
}

}

<TEMPLATE_NAME_BRACKET_STATE>{

"<" {
	itsBracketCount++;
	itsString += "<";
	}

[^<>]+ {
	itsString += text();
	}

">" {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		pop_state();
		}
	}

}



\" {
	itsString = "\"";
	push_state(DOUBLE_QUOTE_STATE);
	}

<DOUBLE_QUOTE_STATE>{

\\.?     |
[^\\\"]+ {
	itsString += text();
	}

\"(\.\.\.)?(", <incomplete sequence "[^>]+">")? {
	itsString += text();
	pop_state();
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		return P_STRING;
		}
	}
}



\' {
	itsString = "\'";
	start(SINGLE_QUOTE_STATE);
	}

<SINGLE_QUOTE_STATE>{

\\.?     |
[^\\\']+ {
	itsString += text();
	}

\' {
	itsString += "\'";
	NEW_TOKEN(itsString);
	start(INITIAL);
	return P_CHAR;
	}
}



"<>" {
	NEW_TOKEN(text());
	return P_EMPTY_BRACKET;
	}

"<>"{WS}= {
	NEW_TOKEN(text());
	return P_EMPTY_BRACKET_EQ;
	}

"<No data fields>" {
	NEW_TOKEN(text());
	return P_NO_DATA_FIELDS;
	}

"<" {
	itsString       = "<";
	itsBracketCount = 1;
	start(BRACKET_STATE);
	}

<BRACKET_STATE>{

"<" {
	itsBracketCount++;
	itsString += "<";
	}

\\.?       |
[^<>\"\\]+ {
	itsString += text();
	}

\" {
	itsString += "\"";
	push_state(DOUBLE_QUOTE_STATE);
	}

">" {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		start(INITIAL);
		return P_BRACKET;
		}
	}

">"{WS}= {
	itsBracketCount--;
	itsString += ">";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		start(INITIAL);
		return P_BRACKET_EQ;
		}
	}
}



\( {
	itsString       = "(";
	itsBracketCount = 1;
	start(PAREN_STATE);
	}

<PAREN_STATE>{

\( {
	itsBracketCount++;
	itsString += "(";
	}

\\.?       |
[^()\"\\]+ {
	itsString += text();
	}

\" {
	itsString += "\"";
	push_state(DOUBLE_QUOTE_STATE);
	}

\) {
	itsBracketCount--;
	itsString += ")";
	if (itsBracketCount == 0)
		{
		NEW_TOKEN(itsString);
		start(INITIAL);
		return P_PAREN_EXPR;
		}
	}
}



[[:space:]]+	/* ignore */

	/* bison allows us to return single characters */

. {
	return text()[0];
}

<<EOF>> {
	itsBracketCount = 0;
	return P_EOF;
	}

%%
