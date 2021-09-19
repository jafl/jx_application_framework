%top {
/*
Copyright © 2013 by John Lindal.

This scanner reads a PHP .ini file and returns CB::Text::INI::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::INI" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBINIStyler

	enum TokenType
	{
		kUnterminatedString = kEOF+1,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kSectionName,

		kKeyOrValue,
		kAssignment,
		kArrayIndex,

		kSingleQuoteString,
		kDoubleQuoteString,

		kComment,

		kError			// place holder for CBINIStyler
	};
}

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)

%%



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
	start(SINGLE_STRING_STATE);
	}

<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\\(.|\n)? |
[^\\\']+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match double quoted strings */



\" {
	StartToken();
	start(DOUBLE_STRING_STATE);
	}

<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kDoubleQuoteString);
	}

\\(.|\n)? |
[^\\\"]+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
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
