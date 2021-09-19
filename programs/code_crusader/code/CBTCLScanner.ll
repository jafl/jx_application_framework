%top {
/*
Copyright © 2001 by John Lindal.

This scanner reads a TCL file and returns CB::Text::TCL::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::TCL" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBTCLStyler

	enum TokenType
	{
		kEmptyVariable = kEOF+1,
		kUnterminatedString,

		kWhitespace,	// must be the one before the first item in type table

		kPredefinedWord,
		kVariable,
		kString,
		kComment,

		kBrace,
		kSquareBracket,
		kParenthesis,
		kSemicolon,

		kOtherWord,
		kError			// place holder for CBTCLStyler
	};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

private:

	JSize	itsBraceDepth;
}

%x WORD_STATE VAR_STATE COMMENT_STATE STRING_STATE

WSCHAR       ([ \v\t\f\r\n])
WS           ({WSCHAR}+|{WSCHAR}*\\\n{WSCHAR}*)

HEXCHAR      ([[:xdigit:]])
OESCCODE     ([0-7]{1,3})
HESCCODE     ([xX]{HEXCHAR}+)
UESCCODE     ([uU]{HEXCHAR}{1,4})
ESCCHAR      (\\(.|{OESCCODE}|{HESCCODE}|{UESCCODE}))

PREDEFWORD   (after|append|array|auto_execok|auto_import|auto_load|auto_mkindex|auto_qualify|auto_reset|bgerror|binary|break|catch|cd|clock|close|concat|continue|dde|encoding|env|eof|error|errorCode|errorInfo|eval|exec|exit|expr|fblocked|fconfigure|fcopy|file|fileevent|filename|flush|for|foreach|format|gets|glob|global|history|if|incr|info|interp|join|lappend|lindex|linsert|list|llength|load|lrange|lreplace|lsearch|lsort|memory|msgcat|namespace|open|package|parray|pid|pkg_mkindex|proc|puts|pwd|re_syntax|read|regexp|registry|regsub|rename|resource|return|scan|seek|set|socket|source|split|string|subst|switch|tcl_endOfWord|tcl_findLibrary|tcl_startOfNextWord|tcl_startOfPreviousWord|tcl_wordBreakAfter|tcl_wordBreakBefore|tcl_library|tcl_patchLevel|tcl_pkgPath|tcl_platform|tcl_precision|tcl_rcFileName|tcl_rcRsrcName|tcl_traceCompile|tcl_traceExec|tcl_wordchars|tcl_nonwordchars|tcl_version|tell|time|trace|unknown|unset|update|uplevel|upvar|variable|vwait|while|bell|bind|bindtags|bitmap|button|canvas|checkbutton|clipboard|colors|cursors|destroy|entry|event|focus|font|frame|grab|grid|image|keysyms|label|listbox|loadTk|lower|menu|menubutton|message|option|options|pack|photo|place|radiobutton|raise|scale|scrollbar|selection|send|text|tk|tk_bisque|tk_chooseColor|tk_chooseDirectory|tk_dialog|tk_focusFollowsMouse|tk_focusNext|tk_focusPrev|tk_getOpenFile|tk_getSaveFile|tk_messageBox|tk_optionMenu|tk_popup|tk_setPalette|tkerror|tk_library|tk_patchLevel|tkPriv|tk_strictMotif|tk_version|tkwait|toplevel|winfo|wm)
%%

"#" {
	StartToken();
	start(COMMENT_STATE);
	itsBraceDepth = 0;
	}

\" {
	StartToken();
	start(STRING_STATE);
	itsBraceDepth = 0;
	}

"$" {
	StartToken();
	start(VAR_STATE);
	}

{PREDEFWORD} {
	StartToken();
	return ThisToken(kPredefinedWord);
	}

[^\][{}()#\";$\\ \v\t\f\r\n]+ {
	StartToken();
	start(WORD_STATE);
	}

{ESCCHAR} |
\\        {
	StartToken();
	start(WORD_STATE);
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
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kOtherWord);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kOtherWord);
	}

}



	/* Match variable */



<VAR_STATE>{

"{"[^}]+"}"   |
(_|\p{L}|\d)+ {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kVariable);
	}

"{}" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kEmptyVariable);
	}

. {
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kOtherWord);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kOtherWord);
	}

}



	/* Match string */



<STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kString);
	}

\\[^}]?    |
[^\\\"{}]+ {
	ContinueToken();
	}

"{" {
	ContinueToken();
	itsBraceDepth++;
	}

\\"}" {
	if (itsBraceDepth == 0)
		{
		matcher().less(1);
		JStyledText::TextRange r = GetCurrentRange();
		r.charRange.last++;
		r.byteRange.last++;	// single byte character
		SetCurrentRange(r);
		start(INITIAL);
		return ThisToken(kUnterminatedString);
		}
	else
		{
		itsBraceDepth--;
		ContinueToken();
		}
	}

"}" {
	if (itsBraceDepth == 0)
		{
		matcher().less(0);
		start(INITIAL);
		return ThisToken(kUnterminatedString);
		}
	else
		{
		itsBraceDepth--;
		ContinueToken();
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match comment */



<COMMENT_STATE>{

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kComment);
	}

\\[^}]? |
[^\\\n{}]+  {
	ContinueToken();
	}

"{" {
	ContinueToken();
	itsBraceDepth++;
	}

\\"}" {
	if (itsBraceDepth == 0)
		{
		matcher().less(1);
		JStyledText::TextRange r = GetCurrentRange();
		r.charRange.last++;
		r.byteRange.last++;	// single byte character
		SetCurrentRange(r);
		start(INITIAL);
		return ThisToken(kComment);
		}
	else
		{
		itsBraceDepth--;
		ContinueToken();
		}
	}

"}" {
	if (itsBraceDepth == 0)
		{
		matcher().less(0);
		start(INITIAL);
		return ThisToken(kComment);
		}
	else
		{
		itsBraceDepth--;
		ContinueToken();
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kComment);
	}

}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%

/******************************************************************************
 BeginScan (virtual)

 *****************************************************************************/

void
CB::Text::TCL::Scanner::BeginScan
	(
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	CBStylingScannerBase::BeginScan(text, startIndex, input);

	itsBraceDepth = 0;
}
