%top {
/*
Copyright © 1998 by Dustin Laurence.
Copyright © 2021 by John Lindal.

This scanner reads a C/C++ file and creates CBClass instances.
*/

#include "CBCClass.h"
#include <JStringIterator.h>
#include <JStack.h>
#include <JFAID.h>
#include <jAssert.h>
}

%option namespace="CB::Tree::C" lexer="Scanner"
%option lex="CreateClasses" token-type="bool"
%option params="const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList"
%option unicode nodefault full freespace

%class {

public:

	virtual	~Scanner();

private:

	JInteger				itsCurlyBraceDepth;
	bool					itsTemplateFlag;
	JInteger				itsTemplateBracketDepth;
	CBClass::InheritType	itsInheritType;
	CBClass*				itsCurrentClass;

	JPtrArray<JString>*					itsUsingList;
	JStack<JInteger, JArray<JInteger>>	itsCurlyBraceStack;

	JString				itsNamespaceStr;
	JPtrArray<JString>*	itsNamespaceStack;

private:

	void ResetState()
	{
		itsTemplateFlag = false;
		itsCurrentClass = nullptr;
	}
}

%init {
	itsUsingList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsUsingList != nullptr );

	itsNamespaceStack = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNamespaceStack != nullptr );
}

%x NAMESPACE_STATE CLASS_NAME_STATE INHERIT_STATE
%x TEMPLATE_PARAM_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*

CKEYWORD     (auto|break|case|const|continue|default|do|else|enum|extern|for|goto|if|inline|register|restrict|return|sizeof|static|struct|switch|typedef|union|volatile|while)
CPPKEYWORD   (alignas|aslignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|bitand|bitor|catch|compl|concept|constexpr|const_cast|decltype|delete|dynamic_cast|explicit|export|false|final|friend|module|mutable|new|noexcept|not|not_eq|operator|or|or_eq|overload|override|private|protected|public|reinterpret_cast|requires|static_assert|static_cast|this|thread_local|throw|true|try|typeid|typename|virtual|xor|xor_eq)
PPKEYWORD    (include|include_next|define|undef|if|ifdef|ifndef|else|elif|endif|line|pragma|error)
CDATATYPE    (bool|char|char16_t|char32_t|double|float|int|long|nullptr|short|signed|unsigned|void|wchar_t)

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)
IDCAR        (_|\p{L}|{UESCCODE})
IDCDR        (_|\p{L}|\d|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(::{ID})*)

%%

%{
	ResetState();

	itsCurlyBraceDepth      = 0;
	itsTemplateBracketDepth = 0;
	itsInheritType          = CBClass::kInheritPrivate;

	itsUsingList->CleanOut();
	itsCurlyBraceStack.Clear();

	itsNamespaceStr.Clear();
	itsNamespaceStack->CleanOut();
%}

template\b {
	itsTemplateFlag      = true;
	itsTemplateBracketDepth = 0;
	push_state(TEMPLATE_PARAM_STATE);
	}

namespace\b {
	itsNamespaceStr.Clear();
	start(NAMESPACE_STATE);
	}

friend{WS}class{WS}{NSID}	/* ignore */

class{WS}{NSID}{MS};		/* ignore */

class\b {
	start(CLASS_NAME_STATE);
	}

using{WS}{NSID}{MS}; {
	JString* s = jnew JString(text()+5, size()-6);
	assert( s != nullptr );
	s->TrimWhitespace();
	itsUsingList->Append(s);
	}

"{" {
	itsCurlyBraceDepth++;
	}

"}" {
	itsCurlyBraceDepth--;
	if (itsCurlyBraceDepth < 0)
		{
		return false;
		}
	else if (itsCurlyBraceDepth == 0 && !itsCurlyBraceStack.IsEmpty())
		{
		itsCurlyBraceDepth = itsCurlyBraceStack.Pop();
		itsNamespaceStack->DeleteElement(1);
		}
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

(L|u8?|U)?\" {
	start(STRING_STATE);
	}

{CKEYWORD}\b   |
{CPPKEYWORD}\b |
{PPKEYWORD}\b  |
{CDATATYPE}\b  {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract namespace */


<NAMESPACE_STATE>{

{NSID} {
	itsNamespaceStr.Set(text(), size());
	}

"{" {
	if (!itsNamespaceStr.IsEmpty())
		{
		itsCurlyBraceStack.Push(itsCurlyBraceDepth);
		itsCurlyBraceDepth = 1;

		itsNamespaceStack->Prepend(itsNamespaceStr);
		}
	else
		{
		itsCurlyBraceDepth++;
		}
	ResetState();
	start(INITIAL);
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


	/* Extract class name and create CBCClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(text(), size());
	for (auto* s : *itsNamespaceStack)
		{
		fullName.Prepend("::");
		fullName.Prepend(*s);
		}

	CBCClass* c = jnew CBCClass(fullName, CBClass::kClassType, fileID, tree);
	assert( c != nullptr );

	c->SetTemplate(itsTemplateFlag);
	classList->Append(c);

	ResetState();
	itsCurrentClass = c;	// must be after ResetState();
	}

: {
	if (itsCurrentClass != nullptr)
		{
		itsInheritType = CBClass::kInheritPrivate;
		start(INHERIT_STATE);
		}
	else
		{
		ResetState();
		start(INITIAL);
		}
	}

"{" {
	if (itsCurrentClass != nullptr)
		{
		itsCurlyBraceStack.Push(itsCurlyBraceDepth);
		itsCurlyBraceDepth = 1;
		itsNamespaceStack->Prepend(itsCurrentClass->GetName());
		}
	else
		{
		itsCurlyBraceDepth++;
		}
	ResetState();
	start(INITIAL);
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

virtual		/* ignore */

public {
	itsInheritType = CBClass::kInheritPublic;
	}

protected {
	itsInheritType = CBClass::kInheritProtected;
	}

private {
	itsInheritType = CBClass::kInheritPrivate;
	}

{NSID}"<"? {
	JString parentName(text(), size());
	if (parentName.GetLastCharacter() == '<')
		{
		itsTemplateBracketDepth = 1;
		push_state(TEMPLATE_PARAM_STATE);

		parentName.Set(text(), size()-1);
		}

	JString s = parentName;
	s.Prepend("::");

	bool found = false;
	while (true)
		{
		for (const auto* c : *itsUsingList)
			{
			if (!c->EndsWith(s))
				{
				continue;
				}

			JString s1 = *c;
			JStringIterator iter(&s1, kJIteratorStartAtEnd);
			iter.RemovePrev(s.GetCharacterCount());
			iter.Invalidate();

			s1.Append("::");
			s1.Append(parentName);

			parentName = s1;
			found      = true;
			break;
			}

		JStringIterator iter(&s, kJIteratorStartAtEnd);
		if (!found && iter.Prev("::") && !iter.AtBeginning())
			{
			iter.RemoveAllNext();
			}
		else
			{
			break;
			}
		}

	itsCurrentClass->AddParent(itsInheritType, parentName);
	}

"{" {
	itsCurlyBraceStack.Push(itsCurlyBraceDepth);
	itsCurlyBraceDepth = 1;
	itsNamespaceStack->Prepend(itsCurrentClass->GetName());
	ResetState();
	start(INITIAL);
	}

"," {
	itsInheritType = CBClass::kInheritPrivate;
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


	/* Ignore template parameters */


<TEMPLATE_PARAM_STATE>{

"<" {
	itsTemplateBracketDepth++;
	}

">" {
	itsTemplateBracketDepth--;
	if (itsTemplateBracketDepth <= 0)
		{
		pop_state();
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
"*"+[^*/\n]*\n		/* ignore everything inside comment */

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


	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */


<STRING_STATE>{

\" |
\n {
	start(INITIAL);
	}

\\(.|\n)? |
[^\\\n\"]+ 		/* ignore everything inside quotes */

}

%%

CB::Tree::C::Scanner::~Scanner()
{
	jdelete itsUsingList;
	jdelete itsNamespaceStack;
}
