%top {
/*
Copyright © 1999 by John Lindal.

This scanner reads a Java file and builds a CBJavaClass.
*/

#include "CBJavaClass.h"
#include <JPtrArray.h>
#include <JStringIterator.h>
#include <JFAID.h>
#include <jAssert.h>
}

%option namespace="CB::Tree::Java" lexer="Scanner"
%option lex="CreateClasses" token-type="bool"
%option params="const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList"
%option unicode nodefault full freespace

%class {

public:

	virtual ~Scanner();

private:

	JString		itsPackage;
	bool		itsAbstractFlag;
	bool		itsPublicFlag;
	bool		itsFinalFlag;
	CBClass*	itsCurrentClass;

	JInteger	itsGenericDepth;
	bool		itsGenericPop;

	JPtrArray<JString>*	itsImportList;

private:

	void ResetState()
	{
		itsAbstractFlag = false;
		itsPublicFlag   = false;
		itsFinalFlag    = false;
		itsCurrentClass = nullptr;
	}
}

%init {
	itsImportList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsImportList != nullptr );
}

%x CLASS_NAME_STATE INHERIT_STATE
%x GENERIC_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\.{ID})*)

OTHER_KEYWORD (break|byvalue|case|cast|catch|ClassLoader|clone|Cloneable|Compiler|const|continue|default|do|else|enum|equals|false|finally|finalize|for|future|generic|getClass|goto|hashCode|if|inner|instanceof|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|POSITIVE_INFINITY|Process|Random|rest|return|Runnable|Runtime|SecurityManager|static|StringTokenizer|super|switch|synchronized|System|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

%%

%{
	ResetState();

	itsGenericDepth = 1;
	itsGenericPop   = false;

	itsPackage.Clear();
	itsImportList->CleanOut();
%}

package{WS}{NSID}{MS}; {
	itsPackage.Set(text()+7, size()-8);
	itsPackage.TrimWhitespace();
	}

import{WS}{NSID}{MS};  {
	JString s(text()+6, size()-7);
	s.TrimWhitespace();
	itsImportList->Append(s);
	}

public\b {
	itsPublicFlag = true;
	}

(protected|private)\b {
	itsPublicFlag = false;
	}

final\b {
	itsFinalFlag = true;
	}

abstract\b {
	itsAbstractFlag = true;
	}

class\b {
	itsAbstractFlag = false;
	start(CLASS_NAME_STATE);
	}

interface\b {
	itsAbstractFlag = true;
	start(CLASS_NAME_STATE);
	}

(extends|implements)\b {
	start(INHERIT_STATE);
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

\" {
	start(STRING_STATE);
	}

{OTHER_KEYWORD}\b {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract class name and create CBJavaClass */


<CLASS_NAME_STATE>{

{ID}({MS}"<")? {
	if (classList->GetElementCount() == 1)
		{
		itsPackage = classList->GetFirstElement()->GetFullName();
		}

	JString fullName(text(), size());
	bool generic = false;
	if (fullName.GetLastCharacter() == '<')
		{
		generic = true;
		fullName.Set(text(), size()-1);
		fullName.TrimWhitespace();
		}

	if (!itsPackage.IsEmpty())
		{
		fullName.Prepend(".");
		fullName.Prepend(itsPackage);
		}

	CBJavaClass* c = jnew CBJavaClass(fullName, CBClass::kClassType, fileID, tree,
									  itsPublicFlag, itsFinalFlag);
	assert( c != nullptr );

	c->SetAbstract(itsAbstractFlag);
	classList->Append(c);

	ResetState();
	itsCurrentClass = c;	// must be after ResetState();

	if (generic)
		{
		c->SetTemplate();
		itsGenericDepth = 1;
		itsGenericPop   = false;
		start(GENERIC_STATE);
		}
	else
		{
		start(INITIAL);
		}
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

extends |
implements	/* ignore */

{NSID}({MS}"<")? {
	if (itsCurrentClass == nullptr)
		{
		return false;
		}

	JString parentName(text(), size());

	bool generic = false;
	if (parentName.GetLastCharacter() == '<')
		{
		generic = true;
		parentName.Set(text(), size()-1);
		parentName.TrimWhitespace();
		}

	JString s = parentName;
	s.Prepend(".");

	bool found = false;
	while (true)
		{
		for (const auto* c : *itsImportList)
			{
			if (!c->EndsWith(s))
				{
				continue;
				}

			JString s1 = *c;
			JStringIterator iter(&s1, kJIteratorStartAtEnd);
			iter.RemovePrev(s.GetCharacterCount());
			iter.Invalidate();

			s1.Append(".");
			s1.Append(parentName);

			parentName = s1;
			found      = true;
			break;
			}

		JStringIterator iter(&s, kJIteratorStartAtEnd);
		if (!found && iter.Prev(".") && !iter.AtBeginning())
			{
			iter.RemoveAllNext();
			}
		else
			{
			break;
			}
		}

	if (!found && !parentName.Contains("."))
		{
		parentName = itsPackage + "." + parentName;
		}

	itsCurrentClass->AddParent(CBClass::kInheritPublic, parentName);

	if (generic)
		{
		itsGenericDepth = 1;
		itsGenericPop   = true;
		push_state(GENERIC_STATE);
		}
	}

"{" {
	ResetState();
	start(INITIAL);
	}

","		/* ignore */

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Ignore generics parameters */


<GENERIC_STATE>{

"<" {
	itsGenericDepth++;
	}

">" {
	itsGenericDepth--;
	if (itsGenericDepth <= 0 && itsGenericPop)
		{
		pop_state();
		}
	else if (itsGenericDepth <= 0)
		{
		start(INITIAL);
		}
	}

[^<>]+	/* ignore */

}


	/* Ignore C comments -- this is the fast comment scanner from the flex man page,
	   since Vern Paxon presumably can optimize such things far better than I can. */


<C_COMMENT_STATE>{

[^*\n]*        |
[^*\n]*\n      |
"*"+[^*/\n]*   |
"*"+[^*/\n]*\n	/* ignore everything inside comment */

"*"+"/" {
	pop_state();
	}

}


	/* Ignore C++ comments */


<CPP_COMMENT_STATE>{

[^\n]+			/* ignore everything inside comment */

\n {
	pop_state();
	}

}


	/* Match strings -- based on the string scanner from the flex manual. */
	/* Java does not support \\\n to continue a string on the next line.  */


<STRING_STATE>{

\" {
	start(INITIAL);
	}

\n {
	start(INITIAL);
	}

\\.? |
[^\\\n\"]+		/* ignore everything inside quotes */

}

%%

CB::Tree::Java::Scanner::~Scanner()
{
	jdelete itsImportList;
}
