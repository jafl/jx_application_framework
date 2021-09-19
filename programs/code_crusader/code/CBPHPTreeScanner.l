%top {
/*
Copyright © 2014 by John Lindal.

This scanner reads a PHP file and builds a CBPHPClass.
*/

#include "CBPHPClass.h"
#include <JString.h>
#include <JStringIterator.h>
#include <JFAID.h>
#include <jAssert.h>
}

%option namespace="CB::Tree::PHP" lexer="Scanner"
%option lex="CreateClass" token-type="bool"
%option params="const JFAID_t fileID, CBTree* tree, CBClass** theClass"
%option unicode nodefault full freespace

%class {

public:

	virtual ~Scanner();

private:

	JString	itsPHPNamespace;
	bool	itsAbstractFlag;
	bool	itsFinalFlag;

	JString					itsUseName;
	JStringPtrMap<JString>*	itsUseMap;
}

%init {
	itsUseMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsUseMap != nullptr );
}

%x NAMESPACE_STATE USE_STATE USE_AS_STATE CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE

WS     [[:space:]]+

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\\{ID})*)

%%

%{
	itsAbstractFlag = false;
	itsFinalFlag    = false;
	*theClass       = nullptr;

	itsPHPNamespace.Clear();
	itsUseName.Clear();
	itsUseMap->CleanOut();
%}

namespace\b {
	start(NAMESPACE_STATE);
	}

use\b {
	start(USE_STATE);
	}

final\b {
	itsFinalFlag = true;
	}

abstract\b {
	itsAbstractFlag = true;
	}

class\b {
	start(CLASS_NAME_STATE);
	}

interface\b {
	start(CLASS_NAME_STATE);
	itsAbstractFlag = true;
	}

(extends|implements)\b {
	start(INHERIT_STATE);
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
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
	itsPHPNamespace.Set(text(), size());
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Extract imported class */


<USE_STATE>{

as\b {
	start(USE_AS_STATE);
	}

{NSID} {
	JString* s = jnew JString(text(), size());
	assert( s != nullptr );
	itsUseMap->SetElement(*s, s, JPtrArrayT::kDelete);
	itsUseName = *s;
	}

; {
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


<USE_AS_STATE>{

{ID} {
	JString* s = jnew JString(text(), size());
	assert( s != nullptr );
	itsUseMap->SetElement(itsUseName, s, JPtrArrayT::kDelete);
	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	return *theClass != nullptr;
	}

}


	/* Extract class name and create CBPHPClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(text(), size());
	if (!itsPHPNamespace.IsEmpty())
		{
		fullName.Prepend("\\");
		fullName.Prepend(itsPHPNamespace);
		}

	*theClass = jnew CBPHPClass(fullName, CBClass::kClassType, fileID, tree, itsFinalFlag);
	assert( *theClass != nullptr );
	(**theClass).SetAbstract(itsAbstractFlag);

	start(INITIAL);
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
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

	JString parentName(text(), size());
	if (parentName.BeginsWith("\\"))
		{
		parentName.Set(text()+1, size()-1);
		}
	else if (!parentName.Contains("\\"))
		{
		JString s = parentName;
		s.Prepend("\\");

		JStringPtrMapCursor<JString> cursor(itsUseMap);
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
	push_state(C_COMMENT_STATE);
	}

"//"|"#" {
	push_state(CPP_COMMENT_STATE);
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
	pop_state();
	}

}


	/* Ignore C++ comments */


<CPP_COMMENT_STATE>{

[^\n]+				/* ignore everything inside comment */

\n {
	pop_state();
	}

}

%%

CB::Tree::PHP::Scanner::~Scanner()
{
	jdelete itsUseMap;
}
