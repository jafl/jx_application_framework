%{
/*
Copyright © 1999 by John Lindal. All rights reserved.

This scanner reads an X11 Compose rule file and returns JXComposeScanner::Rules.
*/

#define _H_JXComposeScannerL

#include <JXComposeScanner.h>
#include <JXComposeRuleList.h>
#include <JString.h>
#include <X11/Xlib.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	void JXComposeScanner::BuildRuleList(istream& input, JXComposeRuleList* ruleList, \
										 const JSize charCount, JBoolean* isCharInWord, \
										 JCharacter* diacriticalMap, JIndex* diacriticalMarkType)
%}

%option c++ yyclass = "JXComposeScanner" prefix = "JXCompose"
%option 8bit nodefault noyywrap

%x INPUT_STATE  INPUT_NAME_STATE 
%x OUTPUT_STATE OUTPUT_NAME_STATE
%x IGNORE_LINE_STATE

WS	[ \t\v\f\r]+

%%

%{
	BEGIN(INITIAL);
	JArray<KeySym> inputSeq;
	JUInt ruleOutput;
	switch_streams(&input, NULL);
%}

[[:space:]]*"<" {
	inputSeq.RemoveAll();
	BEGIN(INPUT_NAME_STATE);
	}

. {
	BEGIN(IGNORE_LINE_STATE);
	}

<*>\n {
	BEGIN(INITIAL);
	}

<*><<EOF>> {
	return;
	}


	/* Extract input sequence */


<INPUT_STATE>{

"<" {
	BEGIN(INPUT_NAME_STATE);
	}

":" {
	BEGIN(OUTPUT_STATE);
	}

{WS}	/* ignore */

. {
	BEGIN(IGNORE_LINE_STATE);
	}

}


	/* Extract keysym name */


<INPUT_NAME_STATE>{

[_[:alnum:]]+ {
	const KeySym keySym = XStringToKeysym(yytext);
	if (keySym != NoSymbol)
		{
		inputSeq.AppendElement(keySym);
		}
	else
		{
		BEGIN(IGNORE_LINE_STATE);
		}
	}

">" {
	BEGIN(INPUT_STATE);
	}

. {
	BEGIN(IGNORE_LINE_STATE);
	}

}


	/* Extract output keysym */


<OUTPUT_STATE>{

\"[^\\]\" {
	ruleList->AddRule(inputSeq, yytext[1]);		/* single character */
	ruleOutput = yytext[1];
	BEGIN(OUTPUT_NAME_STATE);
	}

\"\\[^0-9]\" {
	switch (yytext[2])
		{
		case 'f': yytext[2] = '\f'; break;
		case 'n': yytext[2] = '\n'; break;
		case 'r': yytext[2] = '\r'; break;
		case 't': yytext[2] = '\t'; break;
		case 'v': yytext[2] = '\v'; break;
		}
	ruleList->AddRule(inputSeq, yytext[2]);		/* escaped character */
	ruleOutput = yytext[2];
	BEGIN(OUTPUT_NAME_STATE);
	}

\"\\[0-7]+\" {
	const JString s(yytext+2, yyleng-3);
	const JBoolean ok = s.ConvertToUInt(&ruleOutput, 8);
	assert( ok );
	ruleList->AddRule(inputSeq, ruleOutput);
	BEGIN(OUTPUT_NAME_STATE);
	}

\"\\x[0-9A-Fa-f]+\" {
	const JString s(yytext+3, yyleng-4);
	const JBoolean ok = s.ConvertToUInt(&ruleOutput, 16);
	assert( ok );
	ruleList->AddRule(inputSeq, ruleOutput);
	BEGIN(OUTPUT_NAME_STATE);
	}

{WS}	/* ignore */

. {
	BEGIN(IGNORE_LINE_STATE);
	}

}


	/* Read name to fill in isCharInWord */


<OUTPUT_NAME_STATE>{

[^ \t\n]+ {
	CheckIsCharInWord(yytext, ruleOutput, charCount, isCharInWord,
					  diacriticalMap, diacriticalMarkType);
	BEGIN(IGNORE_LINE_STATE);
	}

[ \t]+	/* ignore */

}


	/* Ignore until end of line */


<IGNORE_LINE_STATE>{

[^\n]+	/* ignore */

}

%%
