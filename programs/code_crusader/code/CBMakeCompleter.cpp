/******************************************************************************
 CBMakeCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBMakeCompleter.h"
#include <jAssert.h>

CBMakeCompleter* CBMakeCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBMakeCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBMakeCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBMakeCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBMakeCompleter::CBMakeCompleter()
	:
	CBStringCompleter(kCBMakeLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBMakeCompleter::~CBMakeCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CBMakeCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
