%top {
/*
Copyright © 1998 by Dustin Laurence.
Copyright © 2021 by John Lindal.

This scanner reads a D file and creates CBClass instances.
*/

#include "CBDClass.h"
#include <JString.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JStringPtrMap.h>
#include <JStack.h>
#include <jFileUtil.h>
#include <JFAID.h>
#include <jAssert.h>
}

%option namespace="CB::Tree::D" lexer="Scanner"
%option lex="CreateClasses" token-type="bool"
%option params="const JString& fileName, const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList"
%option unicode nodefault full freespace

%class {

public:

	virtual ~Scanner();

private:

	JString		itsPackage;
	JInteger	itsCurlyBraceDepth;
	JInteger	itsDepth;
	bool		itsAbstractFlag;
	bool		itsFinalFlag;
	bool		itsTemplateFlag;
	CBClass*	itsCurrentClass;

	JStack<JInteger, JArray<JInteger>>	itsCurlyBraceStack;
	JPtrArray<JString>*					itsNamespaceStack;
	JPtrArray<JString>*					itsAliasKeys;
	JStringPtrMap<JString>*				itsAliasMap;

	JPtrArray<JString>*	itsImportPackages;
	JString				itsImportPackage;
	JString				itsImportAlias;

private:

	void ResetState()
	{
		itsAbstractFlag = false;
		itsFinalFlag    = false;
		itsTemplateFlag = false;
		itsCurrentClass = nullptr;
	}

	void	StoreAlias(const JString& s);
}

%init {
	itsNamespaceStack = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNamespaceStack != nullptr );

	itsAliasKeys = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsAliasKeys != nullptr );

	itsAliasMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsAliasMap != nullptr );

	itsImportPackages = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsImportPackages != nullptr );
}

%x IMPORT_STATE IMPORT_ALIAS_STATE
%x CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE D_COMMENT_STATE
%x STRING_STATE WYSIWYG_STRING_STATE ALT_WYSIWYG_STRING_STATE
%x HEX_STRING_STATE TOKEN_STRING_STATE

WS     [[:space:]]+
MS     [[:space:]]*

KEYWORD   (alias|align|asm|assert|auto|body|break|case|cast|catch|const|continue|debug|default|delegate|delete|deprecated|do|else|enum|export|extern|false|finally|for|foreach|foreach_reverse|function|goto|if|immutable|import|in|inout|invariant|is|lazy|long|macro|mixin|module|new|nothrow|null|out|override|pragma|private|protected|public|pure|ref|return|scope|shared|static|struct|super|switch|synchronized|this|throw|true|try|typeid|typeof|union|unittest|version|void|while|with|__DATE__|__EOF__|__FILE__|__FILE_FULL_PATH__|__FUNCTION__|__LINE__|__MODULE__|__PRETTY_FUNCTION__|__TIME__|__TIMESTAMP__|__VENDOR__|__VERSION__|__gshared|__traits|__vector|__parameters)
DATATYPE  (bool|byte|cdouble|cent|cfloat|char|creal|dchar|double|float|idouble|ifloat|int|ireal|real|short|ubyte|ucent|uint|ulong|ushort|wchar)

IDCAR     (_|\p{L})
IDCDR     (_|\p{L}|\d)
ID        ({IDCAR}{IDCDR}*)
NSID      ({ID}(\.{ID})*)

IDT       ({ID}(!\([^)]+\))?)
NSIDT     ({IDT}(\.{IDT})*)

%%

%{
	ResetState();

	JString path, name, suffix;
	JSplitPathAndName(fileName, &path, &name);
	JSplitRootAndSuffix(name, &itsPackage, &suffix);

	itsCurlyBraceDepth = itsDepth = 0;

	itsImportPackage.Clear();
	itsImportAlias.Clear();

	itsCurlyBraceStack.Clear();
	itsNamespaceStack->CleanOut();
	itsAliasKeys->CleanOut();
	itsAliasMap->CleanOut();
	itsImportPackages->CleanOut();
%}

package{WS}{NSID}{MS}; {
	itsPackage.Set(text()+7, size()-8);
	itsPackage.TrimWhitespace();
	}

import\b  {
	itsImportPackage.Clear();
	itsImportAlias.Clear();
	start(IMPORT_STATE);
	}

alias{WS}{ID}{MS}={MS}{NSIDT}{MS}; {
	JString s(text()+5, size()-6);
	StoreAlias(s);
	}

final\b {
	itsFinalFlag = true;
	}

abstract\b {
	itsAbstractFlag = true;
	}

template\b {
	itsTemplateFlag = true;
	start(CLASS_NAME_STATE);
	}

class\b {
	itsAbstractFlag = false;
	start(CLASS_NAME_STATE);
	}

interface\b {
	itsAbstractFlag = true;
	start(CLASS_NAME_STATE);
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

"/+" {
	itsDepth = 1;
	push_state(D_COMMENT_STATE);
	}

\" {
	start(STRING_STATE);
	}

[rq]\" {
	start(WYSIWYG_STRING_STATE);
	}

\` {
	start(ALT_WYSIWYG_STRING_STATE);
	}

x\" {
	start(HEX_STRING_STATE);
	}

"q{" {
	itsDepth = 1;
	start(TOKEN_STRING_STATE);
	}

{KEYWORD}\b   |
{DATATYPE}\b  {
	ResetState();
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract imported itsPackage */


<IMPORT_STATE>{

{NSID} {
	itsImportPackage.Set(text(), size());
	}

"=" {
	if (!itsImportPackage.IsEmpty())
		{
		itsImportAlias = itsImportPackage;
		itsImportPackage.Clear();
		}
	else
		{
		ResetState();
		start(INITIAL);
		}
	}

[:;] {
	if (!itsImportPackage.IsEmpty())
		{
		if (!itsImportAlias.IsEmpty())
			{
			itsAliasKeys->Append(itsImportAlias);
			itsAliasMap->SetNewElement(itsImportAlias, itsImportPackage);
			}
		itsImportPackages->Append(itsImportPackage);
		start(text()[0] == ':' ? IMPORT_ALIAS_STATE : INITIAL);
		}
	}

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

"/+" {
	itsDepth = 1;
	push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract imported aliases */


<IMPORT_ALIAS_STATE>{

{ID}{MS}={MS}{NSID} {
	JString s(text(), size());
	StoreAlias(s);
	}

{ID}	/* ignore */

","		/* ignore */

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

"/+" {
	itsDepth = 1;
	push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract class name and create CBDClass */


<CLASS_NAME_STATE>{

{ID} {
	JString fullName(text(), size());
	for (auto* s : *itsNamespaceStack)
		{
		fullName.Prepend(".");
		fullName.Prepend(*s);
		}
	fullName.Prepend(".");
	fullName.Prepend(itsPackage);

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

"/+" {
	itsDepth = 1;
	push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
	}

}


	/* Extract base classes */


<INHERIT_STATE>{

{NSID} {
	JString parentName(text(), size());
	JString s;

	JSize count = 0;
	while (true)
		{
		bool changed = false;
		for (const auto* c : *itsAliasKeys)
			{
			JString* s1;
			itsAliasMap->GetElement(*c, &s1);

			if (*c == parentName)
				{
				parentName = *s1;
				changed    = true;
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
				changed = true;
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
	itsCurlyBraceStack.Push(itsCurlyBraceDepth);
	itsCurlyBraceDepth = 1;
	itsNamespaceStack->Prepend(itsCurrentClass->GetName());
	ResetState();
	start(INITIAL);
	}

"," 	/* ignore */

{WS}	/* ignore */

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
	}

"/+" {
	itsDepth = 1;
	push_state(D_COMMENT_STATE);
	}

. {
	ResetState();
	start(INITIAL);
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


	/* Ignore D comments */


<D_COMMENT_STATE>{

[^+/\n]*\n?     |
"+"+[^+/\n]*\n? |
"/"					/* ignore everything inside comment */

"/+" {
	itsDepth++;
	}

"+"+"/" {
	itsDepth--;
	if (itsDepth == 0)
		{
		pop_state();
		}
	}

}


	/* Match strings -- based on the string scanner from the flex manual. */


<STRING_STATE>{

\"[cwd]? {
	start(INITIAL);
	}

\n {
	start(INITIAL);
	}

\\(.|\n)?  |
[^\\\n\"]+		/* ignore everything inside quotes */

}


<WYSIWYG_STRING_STATE>{

\"[cwd]? {
	start(INITIAL);
	}

[^\"]+	/* ignore everything inside quotes */

}


<ALT_WYSIWYG_STRING_STATE>{

\`[cwd]? {
	start(INITIAL);
	}

[^\`]+	/* ignore everything inside quotes */

}


<HEX_STRING_STATE>{

\"[cwd]? {
	start(INITIAL);
	}

[[:xdigit:][:space:]]+	/* ignore everything inside quotes */

[^"] {
	start(INITIAL);
	}
}


<TOKEN_STRING_STATE>{

"}" {
	itsDepth--;
	if (itsDepth == 0)
		{
		start(INITIAL);
		}
	}

"{" {
	itsDepth++;
	}

[^{}]+	/* ignore everything inside quotes */

}

%%

CB::Tree::D::Scanner::~Scanner()
{
	jdelete itsNamespaceStack;
	jdelete itsAliasKeys;
	jdelete itsAliasMap;
	jdelete itsImportPackages;
}

static const JRegex templateParameterPattern("!\\([^)]+\\)");

void
CB::Tree::D::Scanner::StoreAlias
	(
	const JString& s
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

	itsAliasKeys->Append(key);
	itsAliasMap->SetNewElement(*key, value);

	if (!itsNamespaceStack->IsEmpty())
		{
		key = jnew JString(*key);
		assert( key != nullptr );

		value = jnew JString(*value);
		assert( value != nullptr );

		for (auto* s : *itsNamespaceStack)
			{
			key->Prepend(".");
			key->Prepend(*s);
			}

		itsAliasKeys->Append(key);
		itsAliasMap->SetNewElement(*key, value);
		}
}
