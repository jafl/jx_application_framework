/******************************************************************************
 CBEiffelCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBEiffelCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBEiffelCompleter* CBEiffelCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBEiffelCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBEiffelCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
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
	CBStringCompleter(kCBEiffelLang, kKeywordCount, kKeywordList, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEiffelCompleter::~CBEiffelCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBEiffelCompleter::IsWordCharacter
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

	JIndexRange r(1,1);
	while (source.LocateNextSubstring(".", &(r.last)))
		{
		target->MatchCase(source, r, r);
		(r.last)++;
		r.first = r.last;
		}

	JIndexRange r1 = r;
	r1.last = source.GetLength();

	r.last = r.first;
	if (!target->LocateNextSubstring(".", &(r.last)))
		{
		r.last = target->GetLength();
		}

	target->MatchCase(source, r1, r);
}
