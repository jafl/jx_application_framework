%{
/*
Copyright (C) 2001 by John Lindal.

This scanner reads a TCL file and returns CBTCLScanner::Tokens.
*/

#define _H_CBTCLScannerL

#include "CBTCLScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBTCLScanner::Token CBTCLScanner::NextToken()
%}

%option c++ yyclass = "CBTCLScanner" prefix = "CBTCL"
%option 8bit nodefault noyywrap
%option full ecs align

%x WORD_STATE VAR_STATE COMMENT_STATE STRING_STATE

WSCHAR       ([ \v\t\f\r\n])
WS           ({WSCHAR}+|{WSCHAR}*\\\n{WSCHAR}*)

HEXCHAR      ([0-9a-fA-F])
OESCCODE     ([0-7]{1,3})
HESCCODE     ([xX]{HEXCHAR}+)
UESCCODE     ([uU]{HEXCHAR}{1,4})
ESCCHAR      (\\(.|{OESCCODE}|{HESCCODE}|{UESCCODE}))

PREDEFWORD   (after|append|array|auto_execok|auto_import|auto_load|auto_mkindex|auto_qualify|auto_reset|bgerror|binary|break|catch|cd|clock|close|concat|continue|dde|encoding|env|eof|error|errorCode|errorInfo|eval|exec|exit|expr|fblocked|fconfigure|fcopy|file|fileevent|filename|flush|for|foreach|format|gets|glob|global|history|if|incr|info|interp|join|lappend|lindex|linsert|list|llength|load|lrange|lreplace|lsearch|lsort|memory|msgcat|namespace|open|package|parray|pid|pkg_mkindex|proc|puts|pwd|re_syntax|read|regexp|registry|regsub|rename|resource|return|scan|seek|set|socket|source|split|string|subst|switch|tcl_endOfWord|tcl_findLibrary|tcl_startOfNextWord|tcl_startOfPreviousWord|tcl_wordBreakAfter|tcl_wordBreakBefore|tcl_library|tcl_patchLevel|tcl_pkgPath|tcl_platform|tcl_precision|tcl_rcFileName|tcl_rcRsrcName|tcl_traceCompile|tcl_traceExec|tcl_wordchars|tcl_nonwordchars|tcl_version|tell|time|trace|unknown|unset|update|uplevel|upvar|variable|vwait|while|bell|bind|bindtags|bitmap|button|canvas|checkbutton|clipboard|colors|cursors|destroy|entry|event|focus|font|frame|grab|grid|image|keysyms|label|listbox|loadTk|lower|menu|menubutton|message|option|options|pack|photo|place|radiobutton|raise|scale|scrollbar|selection|send|text|tk|tk_bisque|tk_chooseColor|tk_chooseDirectory|tk_dialog|tk_focusFollowsMouse|tk_focusNext|tk_focusPrev|tk_getOpenFile|tk_getSaveFile|tk_messageBox|tk_optionMenu|tk_popup|tk_setPalette|tkerror|tk_library|tk_patchLevel|tkPriv|tk_strictMotif|tk_version|tkwait|toplevel|winfo|wm)
%%

%{
/************************************************************************/

	JSize braceDepth = 0;

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}

"#" {
	StartToken();
	BEGIN(COMMENT_STATE);
	braceDepth = 0;
	}

\" {
	StartToken();
	BEGIN(STRING_STATE);
	braceDepth = 0;
	}

"$" {
	StartToken();
	BEGIN(VAR_STATE);
	}

{PREDEFWORD} {
	StartToken();
	return ThisToken(kPredefinedWord);
	}

[^\][{}()#\";$\\ \v\t\f\r\n]+ {
	StartToken();
	BEGIN(WORD_STATE);
	}

{ESCCHAR} |
\\        {
	StartToken();
	BEGIN(WORD_STATE);
	}

"{" {
	StartToken();
	return ThisToken(kBrace);
	}

"}" {
	StartToken();
	return ThisToken(kBrace);
	}

"[" {
	StartToken();
	return ThisToken(kSquareBracket);
	}

"]" {
	StartToken();
	return ThisToken(kSquareBracket);
	}

"(" {
	StartToken();
	return ThisToken(kParenthesis);
	}

")" {
	StartToken();
	return ThisToken(kParenthesis);
	}

";" {
	StartToken();
	return ThisToken(kSemicolon);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}



	/* Match word */



<WORD_STATE>{

[^\]});$\\ \v\t\f\r\n]+ |
{ESCCHAR}               |
\\                      {
	ContinueToken();
	}

"]"  |
"}"  |
")"  |
";"  |
"$"  |
{WS} {
	yyless(0);
	BEGIN(INITIAL);
	return ThisToken(kOtherWord);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kOtherWord);
	}

}



	/* Match variable */



<VAR_STATE>{

"{"[^}]+"}"   |
[[:alnum:]_]+ {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kVariable);
	}

"{}" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kEmptyVariable);
	}

[^[:alnum:]_] {
	yyless(0);
	BEGIN(INITIAL);
	return ThisToken(kOtherWord);
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kOtherWord);
	}

}



	/* Match string */



<STRING_STATE>{

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kString);
	}

\\[^}]?    |
[^\\\"{}]+ {
	ContinueToken();
	}

"{" {
	ContinueToken();
	braceDepth++;
	}

\\"}" {
	if (braceDepth == 0)
		{
		yyless(1);
		itsCurrentRange.charRange.last++;
		itsCurrentRange.byteRange.last++;	// single byte character
		BEGIN(INITIAL);
		return ThisToken(kUnterminatedString);
		}
	else
		{
		braceDepth--;
		ContinueToken();
		}
	}

"}" {
	if (braceDepth == 0)
		{
		yyless(0);
		BEGIN(INITIAL);
		return ThisToken(kUnterminatedString);
		}
	else
		{
		braceDepth--;
		ContinueToken();
		}
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match comment */



<COMMENT_STATE>{

\n {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kComment);
	}

\\[^}]? |
[^\\\n{}]+  {
	ContinueToken();
	}

"{" {
	ContinueToken();
	braceDepth++;
	}

\\"}" {
	if (braceDepth == 0)
		{
		yyless(1);
		itsCurrentRange.charRange.last++;
		itsCurrentRange.byteRange.last++;	// single byte character
		BEGIN(INITIAL);
		return ThisToken(kComment);
		}
	else
		{
		braceDepth--;
		ContinueToken();
		}
	}

"}" {
	if (braceDepth == 0)
		{
		yyless(0);
		BEGIN(INITIAL);
		return ThisToken(kComment);
		}
	else
		{
		braceDepth--;
		ContinueToken();
		}
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kComment);
	}

}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
