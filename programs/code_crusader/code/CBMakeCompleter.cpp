/******************************************************************************
 CBMakeCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBMakeCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBMakeCompleter* CBMakeCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	"addprefix", "addsuffix",
	"basename",
	"call",
	"define", "dir",
	"else", "endef", "endif", "error", "export",
	"filter", "filter-out", "findstring", "firstword", "foreach",
	"if", "ifdef", "ifeq", "ifndef", "ifneq", "include",
	"join",
	"notdir",
	"origin", "override",
	"patsubst",
	"shell", "sort", "strip", "subst", "suffix",
	"unexport",
	"warning", "wildcard", "word", "wordlist", "words"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBMakeCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBMakeCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBMakeCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBMakeCompleter::CBMakeCompleter()
	:
	CBStringCompleter(kCBMakeLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMakeCompleter::~CBMakeCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBMakeCompleter::IsWordCharacter
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
