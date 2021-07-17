/******************************************************************************
 CBCSharpCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#include "CBCSharpCompleter.h"
#include <jAssert.h>

CBCSharpCompleter* CBCSharpCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"abstract", "as", "base", "bool", "break", "byte",
	"case", "catch", "char", "checked", "class", "const", "continue",
	"decimal", "default", "delegate", "do", "double",
	"else", "enum", "event", "explicit", "extern",
	"false", "finally", "fixed", "float", "for", "foreach", "goto",
	"if", "implicit", "in", "int", "interface", "internal", "is",
	"lock", "long", "namespace", "new", "null",
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBCSharpCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBCSharpCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
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
	CBStringCompleter(kCBCSharpLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCSharpCompleter::~CBCSharpCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBCSharpCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}
