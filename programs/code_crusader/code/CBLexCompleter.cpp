/******************************************************************************
 CBLexCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBLexCompleter.h"
#include "CBCCompleter.h"
#include "CBCStyler.h"
#include <jAssert.h>

CBLexCompleter* CBLexCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBLexCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBLexCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBLexCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBLexCompleter::CBLexCompleter()
	:
	CBStringCompleter(kCBLexLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();	// include C
	ListenTo(CBCStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBLexCompleter::~CBLexCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBLexCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == ':');
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

	const JUtf8Byte** cWordList;
	const JSize count = CBCCompleter::GetDefaultWordList(&cWordList);
	for (JUnsignedOffset i=0; i<count; i++)
		{
		Add(JString(cWordList[i], JString::kNoCopy));
		}

	CopySymbolsForLanguage(kCBCLang);
	CopyWordsFromStyler(CBCStyler::Instance());
}
