%top {
/*
Copyright © 2021 by John Lindal.

This scanner reads a GoLang file and creates CBGoClass instances.
*/

#include "CBGoClass.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <JPtrArray.h>
#include <JStringPtrMap.h>
#include <JFAID.h>
#include <jAssert.h>

static const JRegex modulePattern("(?:.+?/)*(.+?)(?:/[vV][.0-9]+)?");
}

%option namespace="CB::Tree::Go" lexer="Scanner"
%option lex="CreateClasses" token-type="bool"
%option params="const JFAID_t fileID, CBTree* tree, JPtrArray<CBClass>* classList"
%option unicode nodefault full freespace

%class {

public:

	virtual ~Scanner();

private:

	JString					itsPackage;
	JString					itsImportAlias;
	JStringPtrMap<JString>*	itsImportMap;
	JString					itsTypeName;
	JUtf8Character			itsAbstractFlag;
	CBGoClass*				itsCurrentClass;
	JString					itsTypeContent;

private:

	void	ParseTypeContent(CBClass* goClass);
}

%init {
	itsImportMap = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsImportMap != nullptr );
}

%x PACKAGE_STATE IMPORT_STATE CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+

IDCAR  (_|\p{L})
IDCDR  (_|\p{L}|\d)
ID     ({IDCAR}{IDCDR}*)

OTHER_KEYWORD (break|case|chan|const|continue|default|defer|else|fallthrough|false|for|func|go|goto|if|interface|iota|map|nil|range|return|select|struct|switch|true|var)

DATATYPE      (bool|byte|complex64|complex128|error|float32|float64|int|int8|int16|int32|int64|rune|string|uint|uint8|uint16|uint32|uint64|uintptr)

FUNCTION      (append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover)

%%

%{
	itsCurrentClass = nullptr;

	itsPackage.Clear();
	itsImportAlias.Clear();
	itsImportMap->CleanOut();
	itsTypeName.Clear();
	itsAbstractFlag = ' ';
	itsTypeContent.Clear();
%}

package\b {
	start(PACKAGE_STATE);
	}

import\b  {
	itsImportAlias.Clear();
	start(IMPORT_STATE);
	}

type\b {
	start(CLASS_NAME_STATE);
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

`(.|\n)*`	/* ignore raw strings -- no backslash interpretation */

{OTHER_KEYWORD}{WS} |
{DATATYPE}{WS}      |
{FUNCTION}{WS}      {
	itsCurrentClass = nullptr;
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract class itsPackage */


<PACKAGE_STATE>{

{ID} {
	itsPackage.Set(text(), size());
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
	itsCurrentClass = nullptr;
	start(INITIAL);
	}

}


	/* Extract imported class */


<IMPORT_STATE>{

{ID}|\. {
	itsImportAlias.Set(text(), size());
	if (itsImportAlias == "_" || itsImportAlias == ".")
		{
		start(INITIAL);
		}
	itsImportAlias += ".";		// easier lookup
	}

\"[^"]+\" {
	JString s(text()+1, size()-2);
	const JStringMatch m = modulePattern.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty() && !itsImportAlias.IsEmpty())
		{
		itsImportMap->SetNewElement(itsImportAlias, m.GetSubstring(1));
		}
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
	itsCurrentClass = nullptr;
	start(INITIAL);
	}

}


	/* Parse struct/interface and create CBGoClass */


<CLASS_NAME_STATE>{

{ID} {
	itsTypeName.Set(text(), size());
	itsAbstractFlag = ' ';
	}

struct\b {
	itsAbstractFlag = 'F';
	}

interface\b {
	itsAbstractFlag = 'T';
	}

"{" {
	if (itsAbstractFlag == ' ')
		{
		start(INITIAL);
		}
	else
		{
		const bool isExported = itsTypeName.GetFirstCharacter().IsUpper();

		if (!itsPackage.IsEmpty())
			{
			itsTypeName.Prepend(".");
			itsTypeName.Prepend(itsPackage);
			}

		itsCurrentClass = jnew CBGoClass(itsTypeName, CBClass::kClassType, fileID, tree,
									  isExported);
		assert( itsCurrentClass != nullptr );

		itsCurrentClass->SetAbstract(itsAbstractFlag == 'T');
		classList->Append(itsCurrentClass);

		itsTypeContent.Clear();
		start(INHERIT_STATE);
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
	itsCurrentClass = nullptr;
	start(INITIAL);
	}

}


	/* Extract contained types */


<INHERIT_STATE>{

[^/}]+|"/" {
	itsTypeContent += JString(text(), size(), JString::kNoCopy);
	}

"}" {
	ParseTypeContent(itsCurrentClass);
	itsCurrentClass = nullptr;
	start(INITIAL);
	}

"/*" {
	push_state(C_COMMENT_STATE);
	}

"//" {
	push_state(CPP_COMMENT_STATE);
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


	/* Match strings -- based on the string scanner from the flex manual. */
	/* Go does not support \\\n to continue a string on the next line.  */


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

CB::Tree::Go::Scanner::~Scanner()
{
	jdelete itsImportMap;
}

static const JRegex separator(";|\n");
static const JRegex nsid("^\\*?(((_|\\p{L})(_|\\p{L}|\\d)*\\.)?(_|\\p{L})(_|\\p{L}|\\d)*)$");

void
CB::Tree::Go::Scanner::ParseTypeContent
	(
	CBClass* goClass
	)
{
	JPtrArray<JString> lineList(JPtrArrayT::kDeleteAll);
	itsTypeContent.Split(separator, &lineList);

	for (auto* line : lineList)
		{
		line->TrimWhitespace();

		const JStringMatch m = nsid.Match(*line, JRegex::kIncludeSubmatches);
		if (m.IsEmpty())
			{
			continue;
			}
		JString parent = m.GetSubstring(1);

		const JString* itsPackage;
		if (!m.GetUtf8ByteRange(2).IsEmpty() &&
			itsImportMap->GetElement(m.GetSubstring(2), &itsPackage))
			{
			JStringIterator iter(&parent);
			iter.Next(".");
			iter.RemoveAllPrev();
			parent.Prepend(".");
			parent.Prepend(*itsPackage);
			}

		goClass->AddParent(CBClass::kInheritPublic, parent);
		}
}
