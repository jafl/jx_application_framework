%{
/*
Copyright (C) 1998 by Dustin Laurence.
Copyright (C) 2021 by John Lindal.

This scanner reads a C/C++ file and creates CBClass instances.
*/

#define _H_CBDTreeScannerL

#include "CBDTreeScanner.h"
#include "CBDClass.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <JStringPtrMap.h>
#include <JStack.h>
#include <jFileUtil.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBDTreeScanner::CreateClasses( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)

static const JRegex templateParameterPattern("!\\([^)]+\\)");

void
StoreAlias
	(
	const JString&				s,
	const JPtrArray<JString>&	nsStack,
	JPtrArray<JString>*			aliasKeys,
	JStringPtrMap<JString>*		aliasMap
	)
{
	JPtrArray<JString> partList(JPtrArrayT::kForgetAll);
	s.Split("=", &partList);

	JString* key   = partList.GetElement(1);
	JString* value = partList.GetElement(2);
	key->TrimWhitespace();
	value->TrimWhitespace();

	if (value->Contains("!"))
		{
		JStringIterator iter(value);
		while (iter.Next(templateParameterPattern))
			{
			iter.RemoveLastMatch();
			}
		}

	aliasKeys->Append(key);
	aliasMap->SetNewElement(*key, value);

	if (!nsStack.IsEmpty())
		{
		key = jnew JString(*key);
		assert( key != nullptr );

		value = jnew JString(*value);
		assert( value != nullptr );

		for (JString* s : nsStack)
			{
			key->Prepend(".");
			key->Prepend(*s);
			}

		aliasKeys->Append(key);
		aliasMap->SetNewElement(*key, value);
		}
}
%}

%option c++ yyclass="CBDTreeScanner" prefix="CBDTree"
%option 8bit nodefault stack noyywrap

%x IMPORT_STATE IMPORT_ALIAS_STATE
%x CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE D_COMMENT_STATE
%x STRING_STATE WYSIWYG_STRING_STATE ALT_WYSIWYG_STRING_STATE
%x HEX_STRING_STATE TOKEN_STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*
BDRY   ({WS}|[^_[:alnum:]])

KEYWORD   (alias|align|asm|assert|auto|body|break|case|cast|catch|const|continue|debug|default|delegate|delete|deprecated|do|else|enum|export|extern|false|finally|for|foreach|foreach_reverse|function|goto|if|immutable|import|in|inout|invariant|is|lazy|long|macro|mixin|module|new|nothrow|null|out|override|pragma|private|protected|public|pure|ref|return|scope|shared|static|struct|super|switch|synchronized|this|throw|true|try|typeid|typeof|union|unittest|version|void|while|with|__DATE__|__EOF__|__FILE__|__FILE_FULL_PATH__|__FUNCTION__|__LINE__|__MODULE__|__PRETTY_FUNCTION__|__TIME__|__TIMESTAMP__|__VENDOR__|__VERSION__|__gshared|__traits|__vector|__parameters)
DATATYPE  (bool|byte|cdouble|cent|cfloat|char|creal|dchar|double|float|idouble|ifloat|int|ireal|real|short|ubyte|ucent|uint|ulong|ushort|wchar)

IDCAR     ([_[:alpha:]])
IDCDR     ([_[:alnum:]])
ID        ({IDCAR}{IDCDR}*)
NSID      ({ID}(\.{ID})*)

IDT       ({ID}(!\([^)]+\))?)
NSIDT     ({IDT}(\.{IDT})*)

%%

%{
	BEGIN(INITIAL);
	ResetState();

	JString package;
	{
	JString path, name, suffix;
	JSplitPathAndName(fileName, &path, &name);
	JSplitRootAndSuffix(name, &package, &suffix);
	}
	JInteger curlyBraceDepth = 0, depth = 0;
	JStack<JInteger, JArray<JInteger>> curlyBraceStack;
	JPtrArray<JString> nsStack(JPtrArrayT::kDeleteAll);
	JPtrArray<JString> aliasKeys(JPtrArrayT::kDeleteAll);
	JStringPtrMap<JString> aliasMap(JPtrArrayT::kDeleteAll);
	JString importPackage, importAlias;
	JPtrArray<JString> importPackages(JPtrArrayT::kDeleteAll);

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

package{WS}{NSID}{MS}; {
	package.Set(yytext+7, yyleng-8);
	package.TrimWhitespace();
	}

import/{BDRY}  {
	importPackage.Clear();
	importAlias.Clear();
	BEGIN(IMPORT_STATE);
	}

alias{WS}{ID}{MS}={MS}{NSIDT}{MS}; {
	JString s(yytext+5, yyleng-6);
	StoreAlias(s, nsStack, &aliasKeys, &aliasMap);
	}

final/{BDRY} {
	itsFinalFlag = kJTrue;
	}

abstract/{BDRY} {
	itsAbstractFlag = kJTrue;
	}

template/{BDRY} {
	itsTemplateFlag = kJTrue;
	BEGIN(CLASS_NAME_STATE);
	}

class/{BDRY} {
	itsAbstractFlag = kJFalse;
	BEGIN(CLASS_NAME_STATE);
	}

interface/{BDRY} {
	itsAbstractFlag = kJTrue;
	BEGIN(CLASS_NAME_STATE);
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

"/+" {
	depth = 1;
	yy_push_state(D_COMMENT_STATE);
	}

\" {
	BEGIN(STRING_STATE);
	}

[rq]\" {
	BEGIN(WYSIWYG_STRING_STATE);
	}

\` {
	BEGIN(ALT_WYSIWYG_STRING_STATE);
	}

x\" {
	BEGIN(HEX_STRING_STATE);
	}

"q{" {
	depth = 1;
	BEGIN(TOKEN_STRING_STATE);
	}

{KEYWORD}/{BDRY}   |
{DATATYPE}/{BDRY}  {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract imported package */


<IMPORT_STATE>{

{NSID} {
	importPackage.Set(yytext, yyleng);
	}

"=" {
	if (!importPackage.IsEmpty())
		{
		importAlias = importPackage;
		importPackage.Clear();
		}
	else
		{
		ResetState();
		BEGIN(INITIAL);
		}
	}

[:;] {
	if (!importPackage.IsEmpty())
		{
		if (!importAlias.IsEmpty())
			{
			aliasKeys.Append(importAlias);
			aliasMap.SetNewElement(importAlias, importPackage);
			}
		importPackages.Append(importPackage);
		BEGIN(yytext[0] == ':' ? IMPORT_ALIAS_STATE : INITIAL);
		}
	}

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"/+" {
	depth = 1;
	yy_push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract imported aliases */


<IMPORT_ALIAS_STATE>{

{ID}{MS}={MS}{NSID} {
	JString s(yytext, yyleng);
	StoreAlias(s, nsStack, &aliasKeys, &aliasMap);
	}

{ID}	/* ignore */

","		/* ignore */

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"/+" {
	depth = 1;
	yy_push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract class name and create CBDClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(yytext, yyleng);
	for (JString* s : nsStack)
		{
		fullName.Prepend(".");
		fullName.Prepend(*s);
		}
	fullName.Prepend(".");
	fullName.Prepend(package);

	CBDClass* c = jnew CBDClass(fullName, CBClass::kClassType, fileID, tree, itsFinalFlag);
	assert( c != nullptr );

	c->SetAbstract(itsAbstractFlag);
	c->SetTemplate(itsTemplateFlag);
	classList->Append(c);

	ResetState();
	itsCurrentClass = c;	// must be after ResetState();
	}

\([^)]+\)	/* ignore template parameters */

: {
	if (itsCurrentClass != nullptr)
		{
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

"/+" {
	depth = 1;
	yy_push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	BEGIN(INITIAL);
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

{NSID} {
	JString parentName(yytext, yyleng);
	JString s;

	JSize count = 0;
	while (1)
		{
		JBoolean changed = kJFalse;
		for (const JString* c : aliasKeys)
			{
			JString* s1;
			aliasMap.GetElement(*c, &s1);

			if (*c == parentName)
				{
				parentName = *s1;
				changed    = kJTrue;
				break;
				}

			s  = *c;
			s += ".";
			if (parentName.BeginsWith(s))
				{
				JStringIterator iter(&parentName);
				iter.Next(*c);
				iter.RemoveAllPrev();
				parentName.Prepend(*s1);
				changed = kJTrue;
				break;
				}
			}

		count++;
		if (!changed || count > 10)
			{
			break;
			}
		}

	itsCurrentClass->AddParent(CBClass::kInheritPublic, parentName);
	}

"!"?\([^)]+\)	/* ignore template parameters */

"{" {
	curlyBraceStack.Push(curlyBraceDepth);
	curlyBraceDepth = 1;
	nsStack.Prepend(itsCurrentClass->GetName());
	ResetState();
	BEGIN(INITIAL);
	}

"," 	/* ignore */

{WS}	/* ignore */

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
	}

"/+" {
	depth = 1;
	yy_push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	BEGIN(INITIAL);
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


	/* Ignore D comments */


<D_COMMENT_STATE>{

[^+/\n]*\n?     |
"+"+[^+/\n]*\n? |
"/"					/* ignore everything inside comment */

"/+" {
	depth++;
	}

"+"+"/" {
	depth--;
	if (depth == 0)
		{
		yy_pop_state();
		}
	}

}


	/* Match strings -- based on the string scanner from the flex manual. */


<STRING_STATE>{

\"[cwd]? {
	BEGIN(INITIAL);
	}

\n {
	BEGIN(INITIAL);
	}

\\(.|\n)?  |
[^\\\n\"]+		/* ignore everything inside quotes */

}


<WYSIWYG_STRING_STATE>{

\"[cwd]? {
	BEGIN(INITIAL);
	}

[^\"]+	/* ignore everything inside quotes */

}


<ALT_WYSIWYG_STRING_STATE>{

\`[cwd]? {
	BEGIN(INITIAL);
	}

[^\`]+	/* ignore everything inside quotes */

}


<HEX_STRING_STATE>{

\"[cwd]? {
	BEGIN(INITIAL);
	}

[[:xdigit:][:space:]]+	/* ignore everything inside quotes */

[^"] {
	BEGIN(INITIAL);
	}
}


<TOKEN_STRING_STATE>{

"}" {
	depth--;
	if (depth == 0)
		{
		BEGIN(INITIAL);
		}
	}

"{" {
	depth++;
	}

[^{}]+	/* ignore everything inside quotes */

}

%%
