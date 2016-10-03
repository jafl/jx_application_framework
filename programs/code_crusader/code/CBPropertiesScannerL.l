%{
/*
Copyright (C) 2015 by John Lindal. All rights reserved.

This scanner reads a PHP .ini file and returns CBPropertiesScanner::Tokens.
*/

#define _H_CBPropertiesScannerL

#include "CBPropertiesScanner.h"
#include <ctype.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBPropertiesScanner::Token CBPropertiesScanner::NextToken()
%}

%option c++ yyclass = "CBPropertiesScanner" prefix = "CBProperties"
%option 8bit nodefault noyywrap
%option full ecs align

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
