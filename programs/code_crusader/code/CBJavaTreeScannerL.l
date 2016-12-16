%{
/*
Copyright (C) 1999 by John Lindal. All rights reserved.

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
	JBoolean CBJavaTreeScanner::CreateClasses( \
		const JCharacter* fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)
%}

%option c++ yyclass="CBJavaTree" prefix="CBJavaTree"
%option 8bit nodefault stack noyywrap

%x PACKAGE_STATE IMPORT_STATE NAME_STATE INHERIT_STATE
%x GENERIC_STATE C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS	[[:space:]]+

HEXQUAD      ([0-9a-fA-F]{4})
UESCCODE     (\\[uU]{HEXQUAD})
IDCAR        ([_[:alpha:]]|{UESCCODE})
IDCDR        ([_[:alnum:]]|{UESCCODE})
ID           ({IDCAR}{IDCDR}*)
NSID         ({ID}(\.{ID})*)

OTHER_KEYWORD (break|byvalue|case|cast|catch|ClassLoader|clone|Cloneable|Compiler|const|continue|default|do|else|equals|false|finally|finalize|for|future|generic|getClass|goto|hashCode|if|inner|instanceof|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|POSITIVE_INFINITY|Process|Random|rest|return|Runnable|Runtime|SecurityManager|static|StringTokenizer|super|switch|synchronized|System|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

%%

%{
	BEGIN(INITIAL);
	ResetState();

	JString package;
	JPtrArray<JString> importList(JPtrArrayT::kDeleteAll);

	std::ifstream input(fileName);
	switch_streams(&input, NULL);
%}

package{WS} {
	BEGIN(PACKAGE_STATE);
	}

import{WS}  {
	BEGIN(IMPORT_STATE);
	}

public{WS} {
	itsPublicFlag = kJTrue;
	}

protected{WS} |
private{WS}   {
	itsPublicFlag = kJFalse;
	}

final{WS} {
	itsFinalFlag = kJTrue;
	}

abstract{WS} {
	itsAbstractFlag = kJTrue;
	}

class{WS} {
	BEGIN(NAME_STATE);
	}

interface{WS} {
	BEGIN(NAME_STATE);
	itsAbstractFlag = kJTrue;
	}

enum{WS} {
	BEGIN(NAME_STATE);
	}

extends{WS}    |
implements{WS} {
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

{OTHER_KEYWORD}{WS} {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return JNegate( classList->IsEmpty() );
	}


	/* Extract class package */


<PACKAGE_STATE>{

{NSID} {
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
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract imported class */


<IMPORT_STATE>{

{NSID} {
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
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract class name and create CBJavaClass */


<NAME_STATE>{

{NSID}"<"? {
	if (classList->GetElementCount() == 1)
		{
		package = classList->FirstElement()->GetFullName();
		}

	JString fullName(yytext, yyleng);
	if (!JStringEmpty(package))
		{
		fullName.PrependCharacter('.');
		fullName.Prepend(package);
		}

	JBoolean generic = kJFalse;
	if (fullName.GetLastCharacter() == '<')
		{
		generic = kJTrue;
		fullName.RemoveSubstring(fullName.GetLength(), fullName.GetLength());
		}

	CBJavaClass* c = jnew CBJavaClass(fullName, CBClass::kClassType, fileID, tree,
									  itsPublicFlag, itsFinalFlag);
	assert( c != NULL );

	if (itsAbstractFlag)
		{
		c->SetAbstract();
		}

	classList->Append(c);

	ResetState();
	itsCurrentClass = c;	// must be after ResetState();

	if (generic)
		{
		itsGenericDepth = 1;
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

{NSID} {
	if (itsCurrentClass == NULL)
		{
		return kJFalse;
		}

	JString parentName(yytext, yyleng);
	if (parentName != "extends" && parentName != "implements")
		{
		JString s = parentName;
		s.Prepend(".");

		JBoolean found = kJFalse;
		while (1)
			{
			const JSize count = importList.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				const JString* c = importList.NthElement(i);
				if (c->EndsWith(s))
					{
					JString s1 = *c;
					s1.RemoveSubstring(
						s1.GetLength() - s.GetLength() + 1,
						s1.GetLength());
					s1.AppendCharacter('.');
					s1.Append(parentName);

					parentName = s1;
					found      = kJTrue;
					break;
					}
				}

			JIndex dotIndex;
			if (!found && s.LocateLastSubstring(".", &dotIndex) && dotIndex > 1)
				{
				s.RemoveSubstring(dotIndex, s.GetLength());
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
	itsGenericDepth++;
	}

">" {
	itsGenericDepth--;
	if (itsGenericDepth <= 0)
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

\\.?  |
[^\\\n\"]+		/* ignore everything inside quotes */

}

%%
