/******************************************************************************
 CBLexCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBLexCompleter.h"
#include "CBCCompleter.h"
#include "CBCStyler.h"
#include <ctype.h>
#include <jAssert.h>

CBLexCompleter* CBLexCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBLexCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBLexCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBLexCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBLexCompleter::CBLexCompleter()
	:
	CBStringCompleter(kCBLexLang, kKeywordCount, kKeywordList, kJTrue)
{
	UpdateWordList();	// include C
	ListenTo(CBCStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBLexCompleter::~CBLexCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBLexCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == ':'));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBLexCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CBCStyler::Instance() && message.Is(CBStylerBase::kWordListChanged))
		{
		UpdateWordList();
		}
	else
		{
		CBStringCompleter::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateWordList (virtual protected)

 ******************************************************************************/

void
CBLexCompleter::UpdateWordList()
{
	CBStringCompleter::UpdateWordList();

	// include C words

	const JCharacter** cWordList;
	const JSize count = CBCCompleter::GetDefaultWordList(&cWordList);
	for (JIndex i=0; i<count; i++)
		{
		Add(cWordList[i]);
		}

	CopySymbolsForLanguage(kCBCLang);
	CopyWordsFromStyler(CBCStyler::Instance());
}
