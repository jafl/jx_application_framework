/******************************************************************************
 CBPythonCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "CBPythonCompleter.h"
#include <jAssert.h>

CBPythonCompleter* CBPythonCompleter::itsSelf = nullptr;

static const JCharacter* kKeywordList[] =
{
	"and", "access", "break", "class", "continue", "def", "del", "elif",
	"else", "except", "exec", "finally", "for", "from", "global", "if",
	"import", "in", "is", "lambda", "not", "or", "pass", "print", "raise",
	"return", "try", "while", "yield"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBPythonCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPythonCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPythonCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPythonCompleter::CBPythonCompleter()
	:
	CBStringCompleter(kCBPythonLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPythonCompleter::~CBPythonCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBPythonCompleter::IsWordCharacter
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
