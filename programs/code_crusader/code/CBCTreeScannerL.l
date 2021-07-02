%{
/*
Copyright (C) 1998 by Dustin Laurence.
Copyright (C) 2021 by John Lindal.

This scanner reads a C/C++ file and creates CBClass instances.
*/

#define _H_CBCTreeScannerL

#include "CBCTreeScanner.h"
#include "CBCClass.h"
#include <JStringIterator.h>
#include <JStack.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBCTreeScanner::CreateClasses( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)
%}

%option c++ yyclass="CBCTreeScanner" prefix="CBCTree"
%option 8bit nodefault stack noyywrap

%x NAMESPACE_STATE CLASS_NAME_STATE INHERIT_STATE
%x TEMPLATE_PARAM_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*
BDRY   ({WS}|[^_[:alnum:]])

CKEYWORD     (auto|break|case|const|continue|default|do|else|enum|extern|for|goto|if|inline|register|restrict|return|sizeof|static|struct|switch|typedef|union|volatile|while)
CPPKEYWORD   (alignas|aslignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|bitand|bitor|catch|compl|concept|constexpr|const_cast|decltype|delete|dynamic_cast|explicit|export|false|final|friend|module|mutable|new|noexcept|not|not_eq|operator|or|or_eq|overload|override|private|protected|public|reinterpret_cast|requires|static_assert|static_cast|this|thread_local|throw|true|try|typeid|typename|virtual|xor|xor_eq)
PPKEYWORD    (include|include_next|define|undef|if|ifdef|ifndef|else|elif|endif|line|pragma|error)
CDATATYPE    (bool|char|char16_t|char32_t|double|float|int|long|nullptr|short|signed|unsigned|void|wchar_t)

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD}{HEXQUAD}?)
IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(::{ID})*)

%%

%{
	BEGIN(INITIAL);
	ResetState();

	JString namespaceStr;
	JInteger curlyBraceDepth = 0, templateBracketDepth = 0;
	CBClass::InheritType inheritType = CBClass::kInheritPrivate;
	JPtrArray<JString> usingList(JPtrArrayT::kDeleteAll);
	JStack<JInteger, JArray<JInteger>> curlyBraceStack;
	JPtrArray<JString> nsStack(JPtrArrayT::kDeleteAll);

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

template/{BDRY} {
	itsTemplateFlag      = kJTrue;
	templateBracketDepth = 0;
	yy_push_state(TEMPLATE_PARAM_STATE);
	}

namespace/{BDRY} {
	namespaceStr.Clear();
	BEGIN(NAMESPACE_STATE);
	}

class{WS}{NSID}{MS};		/* ignore */

class/{BDRY} {
	BEGIN(CLASS_NAME_STATE);
	}

using{WS}{NSID}{MS}; {
	JString* s = jnew JString(yytext+5, yyleng-6);
	assert( s != nullptr );
	s->TrimWhitespace();
	usingList.Append(s);
	}

"{" {
	curlyBraceDepth++;
	}

"}" {
	curlyBraceDepth--;
	if (curlyBraceDepth < 0)
		{
		return kJFalse;
		}
	else if (curlyBraceDepth == 0 && !curlyBraceStack.IsEmpty())
		{
		curlyBraceDepth = curlyBraceStack.Pop();
		nsStack.DeleteElement(1);
		}
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

(L|u8?|U)?\" {
	BEGIN(STRING_STATE);
	}

{CKEYWORD}/{BDRY}   |
{CPPKEYWORD}/{BDRY} |
{PPKEYWORD}/{BDRY}  |
{CDATATYPE}/{BDRY}  {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract namespace */


<NAMESPACE_STATE>{

{NSID} {
	namespaceStr.Set(yytext, yyleng);
	}

"{" {
	if (!namespaceStr.IsEmpty())
		{
		curlyBraceStack.Push(curlyBraceDepth);
		curlyBraceDepth = 1;

		nsStack.Prepend(namespaceStr);
		}
	else
		{
		curlyBraceDepth++;
		}
	ResetState();
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
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract class name and create CBCClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(yytext, yyleng);
	for (JString* s : nsStack)
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
		inheritType = CBClass::kInheritPrivate;
		BEGIN(INHERIT_STATE);
		}
	else
		{
		ResetState();
		BEGIN(INITIAL);
		}
	}

"{" {
	if (itsCurrentClass != nullptr)
		{
		curlyBraceStack.Push(curlyBraceDepth);
		curlyBraceDepth = 1;
		nsStack.Prepend(itsCurrentClass->GetName());
		}
	else
		{
		curlyBraceDepth++;
		}
	ResetState();
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
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

virtual		/* ignore */

public {
	inheritType = CBClass::kInheritPublic;
	}

protected {
	inheritType = CBClass::kInheritProtected;
	}

private {
	inheritType = CBClass::kInheritPrivate;
	}

{NSID}"<"? {
	JString parentName(yytext, yyleng);
	if (parentName.GetLastCharacter() == '<')
		{
		templateBracketDepth = 1;
		yy_push_state(TEMPLATE_PARAM_STATE);

		parentName.Set(yytext, yyleng-1);
		}

	JString s = parentName;
	s.Prepend("::");

	JBoolean found = kJFalse;
	while (1)
		{
		for (const JString* c : usingList)
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
			found      = kJTrue;
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

	itsCurrentClass->AddParent(inheritType, parentName);
	}

"{" {
	curlyBraceStack.Push(curlyBraceDepth);
	curlyBraceDepth = 1;
	nsStack.Prepend(itsCurrentClass->GetName());
	ResetState();
	BEGIN(INITIAL);
	}

"," {
	inheritType = CBClass::kInheritPrivate;
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

. {
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Ignore template parameters */


<TEMPLATE_PARAM_STATE>{

"<" {
	templateBracketDepth++;
	}

">" {
	templateBracketDepth--;
	if (templateBracketDepth <= 0)
		{
		yy_pop_state();
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


	/* Match strings -- based on the string scanner from the flex manual */
	/* except I allow ANSI wide character strings, may God save my soul. */


<STRING_STATE>{

\" |
\n {
	BEGIN(INITIAL);
	}

\\(.|\n)? |
[^\\\n\"]+ 		/* ignore everything inside quotes */

}

%%
