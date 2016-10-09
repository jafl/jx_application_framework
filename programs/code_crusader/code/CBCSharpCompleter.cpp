/******************************************************************************
 CBCSharpCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCSharpCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBCSharpCompleter* CBCSharpCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	"abstract", "as", "base", "bool", "break", "byte",
	"case", "catch", "char", "checked", "class", "const", "continue",
	"decimal", "default", "delegate", "do", "double",
	"else", "enum", "event", "explicit", "extern",
	"false", "finally", "fixed", "float", "for", "foreach", "goto",
	"if", "implicit", "in", "int", "interface", "internal", "is",
	"lock", "long", "namespace", "jnew", "null",
	"object", "operator", "out", "override",
	"params", "private", "protected", "public",
	"readonly", "ref", "return", "sbyte", "sealed", "short",
	"sizeof", "stackalloc", "static", "string", "struct", "switch",
	"this", "throw", "true", "try", "typeof",
	"uint", "ulong", "unchecked", "unsafe", "ushort", "using",
	"virtual", "volatile", "void", "while",

	// preprocessor

	"if", "else", "elif", "endif", "define", "undef",
	"warning", "error", "line", "region", "endregion",
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBCSharpCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBCSharpCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBCSharpCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBCSharpCompleter::CBCSharpCompleter()
	:
	CBStringCompleter(kCBCSharpLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCSharpCompleter::~CBCSharpCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBCSharpCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == '.'));
}
