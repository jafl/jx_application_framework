%{
/*
Copyright © 2021 by John Lindal.

This scanner reads a GoLang file and creates CBGoClass instances.
*/

#define _H_CBGoTreeScannerL

#include "CBGoTreeScanner.h"
#include "CBGoClass.h"
#include <JRegex.h>
#include <JStringPtrMap.h>
#include <fstream>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL \
	JBoolean CBGoTreeScanner::CreateClasses( \
		const JString& fileName, const JFAID_t fileID, \
		CBTree* tree, JPtrArray<CBClass>* classList)
%}

%option c++ yyclass = "CBGoTree" prefix = "CBGoTree"
%option 8bit nodefault stack noyywrap

%x PACKAGE_STATE IMPORT_STATE CLASS_NAME_STATE INHERIT_STATE
%x C_COMMENT_STATE CPP_COMMENT_STATE STRING_STATE

WS     [[:space:]]+
BDRY   ({WS}|[^_[:alnum:]])

IDCAR  [_[:alpha:]]
IDCDR  [_[:alnum:]]
ID     ({IDCAR}{IDCDR}*)

OTHER_KEYWORD (break|case|chan|const|continue|default|defer|else|fallthrough|false|for|func|go|goto|if|interface|iota|map|nil|range|return|select|struct|switch|true|var)

DATATYPE      (bool|byte|complex64|complex128|error|float32|float64|int|int8|int16|int32|int64|rune|string|uint|uint8|uint16|uint32|uint64|uintptr)

FUNCTION      (append|cap|close|complex|copy|delete|imag|len|make|new|panic|print|println|real|recover)

%%

%{
	BEGIN(INITIAL);

	JString package;
	JString importAlias;
	JStringPtrMap<JString> importMap(JPtrArrayT::kDeleteAll);
	JString typeName;
	JUtf8Character abstract;
	CBGoClass* currentClass = nullptr;
	JString typeContent;

	const JRegex modulePattern("(?:.+?/)*(.+?)(?:/[vV][.0-9]+)?");

	std::ifstream input(fileName.GetBytes());
	switch_streams(&input, nullptr);
%}

package/{BDRY} {
	BEGIN(PACKAGE_STATE);
	}

import/{BDRY}  {
	importAlias.Clear();
	BEGIN(IMPORT_STATE);
	}

type/{BDRY} {
	BEGIN(CLASS_NAME_STATE);
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

`(.|\n)*`	/* ignore raw strings -- no backslash interpretation */

{OTHER_KEYWORD}{WS} |
{DATATYPE}{WS}      |
{FUNCTION}{WS}      {
	currentClass = nullptr;
	}

.|\n	/* ignore */

<*><<EOF>> {
	return ! classList->IsEmpty();
	}


	/* Extract class package */


<PACKAGE_STATE>{

{ID} {
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
	currentClass = nullptr;
	BEGIN(INITIAL);
	}

}


	/* Extract imported class */


<IMPORT_STATE>{

{ID}|\. {
	importAlias.Set(yytext, yyleng);
	if (importAlias == "_" || importAlias == ".")
		{
		BEGIN(INITIAL);
		}
	importAlias += ".";		// easier lookup
	}

\"[^"]+\" {
	JString s(yytext+1, yyleng-2);
	const JStringMatch m = modulePattern.Match(s, kJTrue);
	if (!m.IsEmpty() && !importAlias.IsEmpty())
		{
		importMap.SetNewElement(importAlias, m.GetSubstring(1));
		}
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
	currentClass = nullptr;
	BEGIN(INITIAL);
	}

}


	/* Parse struct/interface and create CBGoClass */


<CLASS_NAME_STATE>{

{ID} {
	typeName.Set(yytext, yyleng);
	abstract = ' ';
	}

struct/{BDRY} {
	abstract = 'F';
	}

interface/{BDRY} {
	abstract = 'T';
	}

"{" {
	if (abstract == ' ')
		{
		BEGIN(INITIAL);
		}
	else
		{
		const JBoolean isExported = typeName.GetFirstCharacter().IsUpper();

		if (!package.IsEmpty())
			{
			typeName.Prepend(".");
			typeName.Prepend(package);
			}

		currentClass = jnew CBGoClass(typeName, CBClass::kClassType, fileID, tree,
									  isExported);
		assert( currentClass != nullptr );

		currentClass->SetAbstract(JI2B(abstract == 'T'));
		classList->Append(currentClass);

		typeContent.Clear();
		BEGIN(INHERIT_STATE);
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
	currentClass = nullptr;
	BEGIN(INITIAL);
	}

}


	/* Extract contained types */


<INHERIT_STATE>{

[^/}]+|"/" {
	typeContent += JString(yytext, yyleng, kJFalse);
	}

"}" {
	ParseTypeContent(currentClass, typeContent, importMap);
	currentClass = nullptr;
	BEGIN(INITIAL);
	}

"/*" {
	yy_push_state(C_COMMENT_STATE);
	}

"//" {
	yy_push_state(CPP_COMMENT_STATE);
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


	/* Match strings -- based on the string scanner from the flex manual. */
	/* Go does not support \\\n to continue a string on the next line.  */


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
