%{
/*
Copyright (C) 1999 by John Lindal.

This scanner reads a Java file and builds a CBJavaClass.
*/

#define _H_CBJavaTreeScannerL

#include "CBJavaTreeScanner.h"
#include "CBJavaClass.h"
#include <JStringIterator.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBJavaTreeScanner::CreateClasses( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)
%}

%option c++ yyclass="CBJavaTreeScanner" prefix="CBJavaTree"
%option 8bit nodefault stack noyywrap

%x CLASS_NAME_STATE INHERIT_STATE
%x GENERIC_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*
BDRY   ({WS}|[^_[:alnum:]])

HEXQUAD      ([[:xdigit:]]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\.{ID})*)

OTHER_KEYWORD (break|byvalue|case|cast|catch|ClassLoader|clone|Cloneable|Compiler|const|continue|default|do|else|enum|equals|false|finally|finalize|for|future|generic|getClass|goto|hashCode|if|inner|instanceof|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|POSITIVE_INFINITY|Process|Random|rest|return|Runnable|Runtime|SecurityManager|static|StringTokenizer|super|switch|synchronized|System|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

%%

%{
	BEGIN(INITIAL);
	ResetState();

	JString package;
	JInteger genericDepth;
	JBoolean genericPop = kJFalse;
	JPtrArray<JString> importList(JPtrArrayT::kDeleteAll);

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

package{WS}{NSID}{MS}; {
	package.Set(yytext+7, yyleng-8);
	package.TrimWhitespace();
	}

import{WS}{NSID}{MS};  {
	JString s(yytext+6, yyleng-7);
	s.TrimWhitespace();
	importList.Append(s);
	}

public/{BDRY} {
	itsPublicFlag = kJTrue;
	}

(protected|private)/{BDRY} {
	itsPublicFlag = kJFalse;
	}

final/{BDRY} {
	itsFinalFlag = kJTrue;
	}

abstract/{BDRY} {
	itsAbstractFlag = kJTrue;
	}

class/{BDRY} {
	itsAbstractFlag = kJFalse;
	BEGIN(CLASS_NAME_STATE);
	}

interface/{BDRY} {
	itsAbstractFlag = kJTrue;
	BEGIN(CLASS_NAME_STATE);
	}

(extends|implements)/{BDRY} {
	BEGIN(INHERIT_STATE);
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

{OTHER_KEYWORD}/{BDRY} {
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
		package = classList->GetFirstElement()->GetFullName();
		}

	JString fullName(yytext, yyleng);
	JBoolean generic = kJFalse;
	if (fullName.GetLastCharacter() == '<')
		{
		generic = kJTrue;
		fullName.Set(yytext, yyleng-1);
		fullName.TrimWhitespace();
		}

	if (!package.IsEmpty())
		{
		fullName.Prepend(".");
		fullName.Prepend(package);
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
		genericDepth = 1;
		genericPop   = kJFalse;
		BEGIN(GENERIC_STATE);
		}
	else
		{
		BEGIN(INITIAL);
		}
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

extends |
implements	/* ignore */

{NSID}({MS}"<")? {
	if (itsCurrentClass == nullptr)
		{
		return kJFalse;
		}

	JString parentName(yytext, yyleng);

	JBoolean generic = kJFalse;
	if (parentName.GetLastCharacter() == '<')
		{
		generic = kJTrue;
		parentName.Set(yytext, yyleng-1);
		parentName.TrimWhitespace();
		}

	JString s = parentName;
	s.Prepend(".");

	JBoolean found = kJFalse;
	while (1)
		{
		for (const JString* c : importList)
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
			found      = kJTrue;
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
		parentName = package + "." + parentName;
		}

	itsCurrentClass->AddParent(CBClass::kInheritPublic, parentName);

	if (generic)
		{
		genericDepth = 1;
		genericPop   = kJTrue;
		yy_push_state(GENERIC_STATE);
		}
	}

"{" {
	ResetState();
	BEGIN(INITIAL);
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
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Ignore generics parameters */


<GENERIC_STATE>{

"<" {
	genericDepth++;
	}

">" {
	genericDepth--;
	if (genericDepth <= 0 && genericPop)
		{
		yy_pop_state();
		}
	else if (genericDepth <= 0)
		{
		BEGIN(INITIAL);
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


	/* Match strings -- based on the string scanner from the flex manual. */
	/* Java does not support \\\n to continue a string on the next line.  */


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
