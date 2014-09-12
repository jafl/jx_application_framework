%{
/*
Copyright © 2014 by John Lindal. All rights reserved.

This scanner reads a PHP file and builds a CBPHPClass.
*/

#define _H_CBPHPTreeScannerL

#include "CBPHPTreeScanner.h"
#include "CBPHPClass.h"
#include <JString.h>
#include <jFStreamUtil.h>
#include <jErrno.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBPHPTreeScanner::CreateClass( \
		const JCharacter* fileName, const JFAID_t fileID, \
		CBTree* tree, CBClass** theClass)
%}

%option c++ yyclass="CBPHPTree" prefix="CBPHPTree"
%option 8bit nodefault stack noyywrap

%x NAMESPACE_STATE USE_STATE USE_AS_STATE NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE

WS	[[:space:]]+

HEXQUAD      ([0-9a-fA-F]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\\{ID})*)

%%

%{
	BEGIN(INITIAL);

	JString phpNamespace, useName;
	JStringPtrMap<JString> useMap(JPtrArrayT::kDeleteAll);
	*theClass           = NULL;
	JBoolean isAbstract = kJFalse;
	JBoolean isFinal    = kJFalse;

	ifstream input(fileName);
	switch_streams(&input, NULL);
%}

namespace{WS} {
	BEGIN(NAMESPACE_STATE);
	}

use{WS} {
	BEGIN(USE_STATE);
	}

final{WS} {
	isFinal = kJTrue;
	}

abstract{WS} {
	isAbstract = kJTrue;
	}

class{WS} {
	BEGIN(NAME_STATE);
	}

interface{WS} {
	BEGIN(NAME_STATE);
	isAbstract = kJTrue;
	}

extends{WS}    |
implements{WS} {
	BEGIN(INHERIT_STATE);
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"{" {
	return JI2B( *theClass != NULL );
	}

.|\n	/* ignore */

<*><<EOF>> {
	return JI2B( *theClass != NULL );
	}


	/* Extract class namespace */


<NAMESPACE_STATE>{

{NSID} {
	phpNamespace.Set(yytext, yyleng);
	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract imported class */


<USE_STATE>{

as{WS} {
	BEGIN(USE_AS_STATE);
	}

{NSID} {
	JString* s = new JString(yytext, yyleng);
	assert( s != NULL );
	useMap.SetElement(*s, s, JPtrArrayT::kDelete);
	useName = *s;
	}

; {
	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


<USE_AS_STATE>{

{ID} {
	JString* s = new JString(yytext, yyleng);
	assert( s != NULL );
	useMap.SetElement(useName, s, JPtrArrayT::kDelete);
	BEGIN(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract class name and create CBPHPClass */


<NAME_STATE>{

{NSID} {
	JString fullName(yytext, yyleng);
	if (!JStringEmpty(phpNamespace))
		{
		fullName.PrependCharacter('\\');
		fullName.Prepend(phpNamespace);
		}

	*theClass = new CBPHPClass(fullName, CBClass::kClassType, fileID, tree, isFinal);
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

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return JI2B( *theClass != NULL );
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

{NSID} {
	if (*theClass == NULL)
		{
		return kJFalse;
		}

	JString parentName(yytext, yyleng);
	if (parentName != "extends" && parentName != "implements")
		{
		if (!parentName.Contains("\\"))
			{
			JString s = parentName;
			s.Prepend("\\");

			JStringPtrMapCursor<JString> cursor(&useMap);
			JString key;
			while (cursor.Next())
				{
				key              = cursor.GetKey();
				const JString* c = cursor.GetValue();
				if (parentName == *c || key.EndsWith(s))
					{
					parentName = key;
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

"//"|"#" {
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
