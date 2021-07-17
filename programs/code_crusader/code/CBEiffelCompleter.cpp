/******************************************************************************
 CBEiffelCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBEiffelCompleter.h"
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jAssert.h>

CBEiffelCompleter* CBEiffelCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"alias", "all", "and", "as", "check", "class", "create", "creation",
	"current", "debug", "deferred", "do", "else", "elseif", "end", "ensure",
	"expanded", "export", "external", "false", "feature", "from", "frozen",
	"if", "implies", "indexing", "infix", "inherit", "inspect", "invariant",
	"is", "like", "local", "loop", "not", "obsolete", "old", "once", "or",
	"prefix", "redefine", "rename", "require", "rescue", "result", "retry",
	"select", "separate", "strip", "then", "true", "undefine", "unique",
	"until", "variant", "when", "xor",

	// data types

	"BIT", "BOOLEAN", "CHARACTER", "DOUBLE", "INTEGER", "NONE", "POINTER",
	"REAL", "STRING"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBEiffelCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBEiffelCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBEiffelCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBEiffelCompleter::CBEiffelCompleter()
	:
	CBStringCompleter(kCBEiffelLang, kKeywordCount, kKeywordList, JString::kIgnoreCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEiffelCompleter::~CBEiffelCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBEiffelCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}

/******************************************************************************
 MatchCase (virtual protected)

	We match the case of each piece between namespace operators separately.

 ******************************************************************************/

void
CBEiffelCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	target->ToLower();

	JStringIterator iter(source);
	iter.BeginMatch();
	while (iter.Next("."))
		{
		const JStringMatch& m = iter.FinishMatch();
		target->MatchCase(source, m.GetCharacterRange(), m.GetCharacterRange());
		iter.BeginMatch();
		}

	const JStringMatch& m = iter.FinishMatch();
	const JCharacterRange r(m.GetCharacterRange().first, source.GetCharacterCount());
	target->MatchCase(source, r, m.GetCharacterRange());
}
