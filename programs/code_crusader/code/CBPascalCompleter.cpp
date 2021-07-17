/******************************************************************************
 CBPascalCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBPascalCompleter.h"
#include <jAssert.h>

CBPascalCompleter* CBPascalCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"and", "array", "begin", "case", "const", "div", "do", "downto",
	"else", "end", "file", "for", "forward", "function", "goto", "if",
	"in", "label", "mod", "nil", "not", "of", "or", "packed",
	"procedure", "program", "record", "repeat", "set", "then", "to",
	"type", "until", "var", "while", "with",

	// types

	"real", "integer", "char", "boolean", "text",

	// functions

	"abs", "sqr", "sqrt", "sin", "cos", "arctan", "ln", "exp",
	"trunc", "round",
	"ord", "chr", "succ", "pred",
	"odd", "eoln", "eof",

	// procedures

	"read", "readln", "write", "writeln",
	"rewrite", "reset", "put", "get", "page",
	"new", "dispose",
	"pack", "unpack"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBPascalCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBPascalCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPascalCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPascalCompleter::CBPascalCompleter()
	:
	CBStringCompleter(kCBPascalLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPascalCompleter::~CBPascalCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CBPascalCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
