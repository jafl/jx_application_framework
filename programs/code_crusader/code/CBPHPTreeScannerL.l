%{
/*
Copyright © 2014 by John Lindal.

This scanner reads a PHP file and builds a CBPHPClass.
*/

#define _H_CBPHPTreeScannerL

#include "CBPHPTreeScanner.h"
#include "CBPHPClass.h"
#include <JStringIterator.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	bool CBPHPTreeScanner::CreateClass( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, CBClass** theClass)
%}

%option c++ yyclass="CBPHPTreeScanner" prefix="CBPHPTree"
%option 8bit nodefault stack noyywrap

%x NAMESPACE_STATE USE_STATE USE_AS_STATE CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE

WS     [[:space:]]+
BDRY   ({WS}|[^_[:alnum:]])

HEXQUAD      ([[:xdigit:]]{4})
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
	*theClass           = nullptr;
	bool isAbstract = false;
	bool isFinal    = false;

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

namespace/{BDRY} {
	BEGIN(NAMESPACE_STATE);
	}

use/{BDRY} {
	BEGIN(USE_STATE);
	}

final/{BDRY} {
	isFinal = true;
	}

abstract/{BDRY} {
	isAbstract = true;
	}

class/{BDRY} {
	BEGIN(CLASS_NAME_STATE);
	}

interface/{BDRY} {
	BEGIN(CLASS_NAME_STATE);
	isAbstract = true;
	}

(extends|implements)/{BDRY} {
	BEGIN(INHERIT_STATE);
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"{" {
	return *theClass != nullptr;
	}

.|\n	/* ignore */

<*><<EOF>> {
	return *theClass != nullptr;
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
	return *theClass != nullptr;
	}

}


	/* Extract imported class */


<USE_STATE>{

as/{BDRY} {
	BEGIN(USE_AS_STATE);
	}

{NSID} {
	JString* s = jnew JString(yytext, yyleng);
	assert( s != nullptr );
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
	return *theClass != nullptr;
	}

}


<USE_AS_STATE>{

{ID} {
	JString* s = jnew JString(yytext, yyleng);
	assert( s != nullptr );
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
	return *theClass != nullptr;
	}

}


	/* Extract class name and create CBPHPClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(yytext, yyleng);
	if (!phpNamespace.IsEmpty())
		{
		fullName.Prepend("\\");
		fullName.Prepend(phpNamespace);
		}

	*theClass = jnew CBPHPClass(fullName, CBClass::kClassType, fileID, tree, isFinal);
	assert( *theClass != nullptr );
	(**theClass).SetAbstract(isAbstract);

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
	return *theClass != nullptr;
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

extends |
implements	/* ignore */

{NSID} {
	if (*theClass == nullptr)
		{
		return false;
		}

	JString parentName(yytext, yyleng);
	if (parentName.BeginsWith("\\"))
		{
		parentName.Set(yytext+1, yyleng-1);
		}
	else if (!parentName.Contains("\\"))
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

","		/* ignore */

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
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
