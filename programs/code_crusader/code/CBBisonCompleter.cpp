/******************************************************************************
 CBBisonCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBBisonCompleter.h"
#include "CBCCompleter.h"
#include "CBCStyler.h"
#include <ctype.h>
#include <jAssert.h>

CBBisonCompleter* CBBisonCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBBisonCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBBisonCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBBisonCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBBisonCompleter::CBBisonCompleter()
	:
	CBStringCompleter(kCBBisonLang, kKeywordCount, kKeywordList, kJTrue)
{
	UpdateWordList();	// include C
	ListenTo(CBCStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBBisonCompleter::~CBBisonCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBBisonCompleter::IsWordCharacter
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
CBBisonCompleter::Receive
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
CBBisonCompleter::UpdateWordList()
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
