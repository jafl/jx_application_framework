/******************************************************************************
 CBPascalCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBPascalCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBPascalCompleter* CBPascalCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBPascalCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBPascalCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPascalCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPascalCompleter::CBPascalCompleter()
	:
	CBStringCompleter(kCBPascalLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPascalCompleter::~CBPascalCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBPascalCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_');
}
