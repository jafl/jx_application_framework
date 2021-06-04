%{
/*
Copyright (C) 2021 by John Lindal.

This scanner reads a GoLang file and creates CBGoClass instances.
*/

#define _H_CBGoTreeScannerL

#include "CBGoTreeScanner.h"
#include "CBGoClass.h"
#include <JStringIterator.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBGoTreeScanner::CreateClasses( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)
%}

%option c++ yyclass = "CBPythonTree" prefix = "CBPythonTree"
%option 8bit nodefault stack noyywrap

%x C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS	[[:space:]]+
MS	[[:space:]]*

IDCAR        [_[:alpha:]]
IDCDR        [_[:alnum:]]
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(/{ID})*)

OTHER_KEYWORD  (x)

%%

%{
	BEGIN(INITIAL);
	ResetState();

	JString package;
	JPtrArray<JString> importList(JPtrArrayT::kDeleteAll);

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

package{WS} {
	BEGIN(PACKAGE_STATE);
	}

import{WS}  {
	BEGIN(IMPORT_STATE);
	}

struct{WS} {
	BEGIN(NAME_STATE);
	}

interface{WS} {
	BEGIN(NAME_STATE);
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

\" {
	BEGIN(STRING_STATE);
	}

`(.|\n)*`	/* ignore raw strings -- no backslash interpretation */

{OTHER_KEYWORD}{WS} {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Ignore C comments -- this is the fast comment scanner from the flex man page,
	   since Vern Paxon presumably can optimize such things far better than I can. */


<C_COMMENT_STATE>{

[^*\n]*        |
[^*\n]*\n      |
"*"+[^*/\n]*   |
"*"+[^*/\n]*\n		/* ignore everything inside comment */

"*"+"/" {
	yy_pop_state();
	}

}


	/* Ignore C++ comments */


<CPP_COMMENT_STATE>{

[^\n]+				/* ignore everything inside comment */

\n {
	yy_pop_state();
	}

}


	/* Match strings -- based on the string scanner from the flex manual. */
	/* Go does not support \\\n to continue a string on the next line.  */


<STRING_STATE>{

\" {
	BEGIN(INITIAL);
	}

\n {
	BEGIN(INITIAL);
	}

\\.? |
[^\\\n\"]+		/* ignore everything inside quotes */

}

%%
