%top {
/*
Copyright © 2015 by John Lindal.

This scanner reads a .properties file and returns CB::Properties::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Properties" lexer="Scanner" prefix="allow_multiple_includes"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBPropertiesStyler

	enum TokenType
	{
		kNonPrintChar = kEOF+1,

		kWhitespace,	// must be the one before the first item in type table

		kKeyOrValue,
		kAssignment,

		kComment,

		kError			// place holder for CBPropertiesStyler
	};
}

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)

%%



	/* Match normal tokens */



[^#= \v\t\f\r]+ {
	StartToken();
	return ThisToken(kKeyOrValue);
	}

= {
	StartToken();
	return ThisToken(kAssignment);
	}



	/* Match comments */



^{WS}"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}
	/* Miscellaneous */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end */
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
