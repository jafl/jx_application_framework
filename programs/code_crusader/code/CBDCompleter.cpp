/******************************************************************************
 CBDCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "CBDCompleter.h"
#include <jAssert.h>

CBDCompleter* CBDCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"abstract", "alias", "align", "asm", "assert", "auto", "body", "bool",
	"break", "byte", "case", "cast", "catch", "cdouble", "cent", "cfloat",
	"char", "class", "const", "continue", "creal", "dchar", "debug",
	"default", "delegate", "delete", "deprecated", "do", "double", "else",
	"enum", "export", "extern", "false", "final", "finally", "float",
	"for", "foreach", "foreach_reverse", "function", "goto", "idouble",
	"if", "ifloat", "immutable", "import", "in", "inout", "int",
	"interface", "invariant", "ireal", "is", "lazy", "long", "macro",
	"mixin", "module", "new", "nothrow", "null", "out", "override",
	"package", "pragma", "private", "protected", "public", "pure", "real",
	"ref", "return", "scope", "shared", "short", "static", "struct", "super",
	"switch", "synchronized", "template", "this", "throw", "true", "try",
	"typeid", "typeof", "ubyte", "ucent", "uint", "ulong", "union",
	"unittest", "ushort", "version", "void", "wchar", "while", "with",

	"__DATE__", "__EOF__", "__FILE__", "__FILE_FULL_PATH__", "__FUNCTION__",
	"__LINE__", "__MODULE__", "__PRETTY_FUNCTION__", "__TIME__",
	"__TIMESTAMP__", "__VENDOR__", "__VERSION__",

	"__gshared", "__traits", "__vector", "__parameters"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBDCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBDCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBDCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBDCompleter::CBDCompleter()
	:
	CBStringCompleter(kCBDLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDCompleter::~CBDCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBDCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
CBDCompleter::GetDefaultWordList
	(
	const JUtf8Byte*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
