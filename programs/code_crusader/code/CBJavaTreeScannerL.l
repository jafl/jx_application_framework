%{
/*
Copyright © 1999 by John Lindal. All rights reserved.

This scanner reads a Java file and builds a CBJavaClass.
*/

#define _H_CBJavaTreeScannerL

#include "CBJavaTreeScanner.h"
#include "CBJavaClass.h"
#include <JString.h>
#include <jFStreamUtil.h>
#include <jErrno.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBJavaTreeScanner::CreateClass( \
		const JCharacter* fileName, const JFAID_t fileID, \
		CBTree* tree, CBClass** theClass)
%}

%option c++ yyclass="CBJavaTree" prefix="CBJavaTree"
%option 8bit nodefault stack noyywrap

%x PACKAGE_STATE IMPORT_STATE NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE

WS	[[:space:]]+

HEXQUAD      ([0-9a-fA-F]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)

%%

%{
	BEGIN(INITIAL);

	JString package;
	JPtrArray<JString> importList(JPtrArrayT::kDeleteAll);
	*theClass           = NULL;
	JBoolean isAbstract = kJFalse;
	JBoolean isPublic   = kJFalse;
	JBoolean isFinal    = kJFalse;

	ifstream input(fileName);
	switch_streams(&input, NULL);
%}

package {
	BEGIN(PACKAGE_STATE);
	}

import  {
	BEGIN(IMPORT_STATE);
	}

public {
	isPublic = kJTrue;
	}

final {
	isFinal = kJTrue;
	}

abstract {
	isAbstract = kJTrue;
	}

class {
	BEGIN(NAME_STATE);
	}

interface {
	BEGIN(NAME_STATE);
	isAbstract = kJTrue;
	}

enum {
	BEGIN(NAME_STATE);
	}

extends    |
implements {
	BEGIN(INHERIT_STATE);
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"{" {
	return JI2B( *theClass != NULL );
	}

.|\n	/* ignore */

<*><<EOF>> {
	return JI2B( *theClass != NULL );
	}


	/* Extract class package */


<PACKAGE_STATE>{

{ID}(\.{ID})* {
	package.Set(yytext, yyleng);
	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract imported class */


<IMPORT_STATE>{

{ID}(\.{ID})* {
	JString s(yytext, yyleng);
	importList.Append(s);
	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract class name and create CBJavaClass */


<NAME_STATE>{

{ID}(\.{ID})* {
	JString fullName(yytext, yyleng);
	if (!JStringEmpty(package))
		{
		fullName.PrependCharacter('.');
		fullName.Prepend(package);
		}

	*theClass = new CBJavaClass(fullName, CBClass::kClassType, fileID, tree,
								isPublic, isFinal);
	assert( *theClass != NULL );

	if (isAbstract)
		{
		(**theClass).SetAbstract();
		}

	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

{ID}(\.{ID})* {
	if (*theClass == NULL)
		{
		return kJFalse;
		}

	JString parentName(yytext, yyleng);
	if (parentName != "extends" && parentName != "implements")
		{
		if (!parentName.Contains("."))
			{
			JString s = parentName;
			s.Prepend(".");

			const JSize count = importList.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				const JString* c = importList.NthElement(i);
				if (c->EndsWith(s))
					{
					parentName = *c;
					break;
					}
				}
			}

		(**theClass).AddParent(CBClass::kInheritPublic, parentName);
		}
	}

","		/* ignore */

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

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

%%
