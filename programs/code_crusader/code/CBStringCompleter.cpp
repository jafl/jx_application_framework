/******************************************************************************
 CBStringCompleter.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBStringCompleter.h"
#include "CBStylerBase.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBSymbolList.h"
#include "cbGlobals.h"
#include <JXStringCompletionMenu.h>
#include <JTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStringCompleter::CBStringCompleter
	(
	const CBLanguage	lang,
	const JSize			keywordCount,
	const JCharacter**	keywordList,
	const JBoolean		caseSensitive
	)
	:
	itsLanguage(lang),
	itsPredefKeywordCount(keywordCount),
	itsPrefefKeywordList(keywordList),
	itsCaseSensitiveFlag(caseSensitive)
{
	itsStringList = jnew JPtrArray<JString>(JPtrArrayT::kForgetAll, 1000);
	assert( itsStringList != nullptr );
	itsStringList->SetSortOrder(JListT::kSortAscending);

	if (itsCaseSensitiveFlag)
		{
		itsStringList->SetCompareFunction(JCompareStringsCaseSensitive);
		}
	else
		{
		itsStringList->SetCompareFunction(JCompareStringsCaseInsensitive);
		}

	itsOwnedList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsOwnedList != nullptr );

	if (itsLanguage != kCBOtherLang)
		{
		ListenTo(CBGetDocumentManager());
		}

	CBGetStyler(lang, &itsStyler);
	UpdateWordList();			// lgtm[cpp/virtual-call-in-constructor]
	if (itsStyler != nullptr)
		{
		ListenTo(itsStyler);
		}

	// We are constructed at a random point in time, so we have to
	// ListenTo() all pre-existing project documents.

	JPtrArray<CBProjectDocument>* docList =
		(CBGetDocumentManager())->GetProjectDocList();

	const JSize docCount = docList->GetElementCount();
	for (JIndex i=1; i<=docCount; i++)
		{
		ListenTo(((docList->GetElement(i))->GetSymbolDirector())->GetSymbolList());
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBStringCompleter::~CBStringCompleter()
{
	jdelete itsStringList;
	jdelete itsOwnedList;
}

/******************************************************************************
 Reset

 ******************************************************************************/

void
CBStringCompleter::Reset()
{
	RemoveAll();

	for (JUnsignedOffset i=0; i<itsPredefKeywordCount; i++)
		{
		Add(itsPrefefKeywordList[i]);
		}
}

/******************************************************************************
 Add

 ******************************************************************************/

void
CBStringCompleter::Add
	(
	const JCharacter* str
	)
{
	JString* s = jnew JString(str);
	assert( s != nullptr );
	if (itsStringList->InsertSorted(s, kJFalse))
		{
		itsOwnedList->Append(s);
		}
	else
		{
		jdelete s;
		}
}

// private

void
CBStringCompleter::Add
	(
	JString* s
	)
{
	itsStringList->InsertSorted(s, kJFalse);
}

/******************************************************************************
 Remove

 ******************************************************************************/
/*
void
CBStringCompleter::Remove
	(
	const JCharacter* str
	)
{
	JString target = str;
	JIndex i;
	if (itsStringList->SearchSorted(&target, JListT::kAnyMatch, &i))
		{
		itsStringList->RemoveElement(i);
		}
}
*/
/******************************************************************************
 RemoveAll

 ******************************************************************************/

void
CBStringCompleter::RemoveAll()
{
	itsStringList->RemoveAll();
	itsOwnedList->DeleteAll();
}

/******************************************************************************
 Complete

	Returns kJTrue if the text behind the caret was completed.

 ******************************************************************************/

JBoolean
CBStringCompleter::Complete
	(
	JTextEditor*			te,
	JXStringCompletionMenu*	menu
	)
{
	return Complete(te, kJTrue, menu);
}

// private

JBoolean
CBStringCompleter::Complete
	(
	JTextEditor*			te,
	const JBoolean			includeNS,
	JXStringCompletionMenu*	menu
	)
{
	assert( menu != nullptr );

	JIndex caretIndex;
	if (!te->GetCaretLocation(&caretIndex))
		{
		return kJFalse;
		}

	const JString& text = te->GetText();
	JIndex i            = caretIndex;
	while (i > 1 && IsWordCharacter(text, i-1, includeNS))
		{
		i--;
		}

	if (i == caretIndex)
		{
		return kJFalse;
		}

	JString s;
	const JString prefix   = text.GetSubstring(i, caretIndex-1);
	const JSize matchCount = Complete(prefix, &s, menu);
	if (matchCount > 0 &&
		s.GetLength() > prefix.GetLength())
		{
		s.RemoveSubstring(1, prefix.GetLength());
		te->Paste(s);
		menu->ClearRequestCount();
		return kJTrue;
		}
	else if (matchCount > 1)
		{
		menu->CompletionRequested(prefix.GetLength());
		return kJTrue;
		}
	else if (matchCount == 0 && includeNS)
		{
		// try once more without namespace
		return Complete(te, kJFalse, menu);
		}

	return kJFalse;
}

/******************************************************************************
 Complete (private)

	Checks if prefix is the beginning of one of our strings.

	If not, returns 0.
	If there is a single match, returns 1 and *maxPrefix contains the string.
	If there are multiple matches, returns how many and *maxPrefix contains
		the most characters that are common to all the matches.

	If there is an exact match, we still check for additional matches so
	the menu can give the user a choice.

 ******************************************************************************/

JSize
CBStringCompleter::Complete
	(
	const JString&			prefix,
	JString*				maxPrefix,
	JXStringCompletionMenu*	menu
	)
	const
{
	menu->RemoveAllItems();

	JBoolean found;
	const JIndex startIndex =
		itsStringList->SearchSorted1(const_cast<JString*>(&prefix),
									 JListT::kAnyMatch, &found);

	const JSize stringCount = itsStringList->GetElementCount();
	if (startIndex > stringCount)
		{
		maxPrefix->Clear();
		return 0;
		}

	JPtrArray<JString> matchList(JPtrArrayT::kForgetAll, 100);
	*maxPrefix = *(itsStringList->GetElement(startIndex));

	JSize matchCount   = 0;
	JBoolean addString = kJTrue;
	for (JIndex i=startIndex; i<=stringCount; i++)
		{
		const JString* s = itsStringList->GetElement(i);
		if (!s->BeginsWith(prefix, itsCaseSensitiveFlag))
			{
			break;
			}

		if (matchCount > 0)
			{
			const JSize matchLength  = JCalcMatchLength(*maxPrefix, *s, itsCaseSensitiveFlag);
			const JSize prefixLength = maxPrefix->GetLength();
			if (matchLength < prefixLength)
				{
				maxPrefix->RemoveSubstring(matchLength+1, prefixLength);
				}
			}

		matchCount++;
		if (itsCaseSensitiveFlag)
			{
			matchList.Append(const_cast<JString*>(s));
			}
		else if (addString)
			{
			JString s1 = *s;
			MatchCase(prefix, &s1);
			addString = menu->AddString(s1);	// must process all to get maxPrefix
			}
		}

	if (itsCaseSensitiveFlag && matchCount > 0)
		{
		matchList.SetSortOrder(JListT::kSortAscending);
		matchList.SetCompareFunction(JCompareStringsCaseInsensitive);
		matchList.Sort();

		assert( matchCount == matchList.GetElementCount() );
		for (JIndex i=1; i<=matchCount; i++)
			{
			if (!menu->AddString(*(matchList.GetElement(i))))
				{
				matchCount = i-1;
				break;
				}
			}
		}
	else if (!itsCaseSensitiveFlag)
		{
		MatchCase(prefix, maxPrefix);
		}

	return matchCount;
}

/******************************************************************************
 MatchCase (virtual protected)

	Adjust target to match the case of source.  target is guaranteed to
	be at least as long as source.

	This is not virtual because only case-insensitive languages need to
	override it.

 ******************************************************************************/

void
CBStringCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	assert_msg( 0, "Programmer forgot to override CBStringCompleter::MatchCase()" );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBStringCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CBGetDocumentManager() &&
		message.Is(CBDocumentManager::kProjectDocumentCreated))
		{
		const CBDocumentManager::ProjectDocumentCreated* info =
			dynamic_cast<const CBDocumentManager::ProjectDocumentCreated*>(&message);
		UpdateWordList();
		ListenTo(((info->GetProjectDocument())->GetSymbolDirector())->GetSymbolList());
		}
	else if (sender == CBGetDocumentManager() &&
			 message.Is(CBDocumentManager::kProjectDocumentDeleted))
		{
		UpdateWordList();
		}

	else if (message.Is(CBSymbolList::kChanged))
		{
		UpdateWordList();
		}

	else if (sender == itsStyler && message.Is(CBStylerBase::kWordListChanged))
		{
		UpdateWordList();
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateWordList (virtual protected)

 ******************************************************************************/

void
CBStringCompleter::UpdateWordList()
{
	// start with predefined keywords

	Reset();

	// add words from styler's override list

	if (itsStyler != nullptr)
		{
		CopyWordsFromStyler(itsStyler);
		}

	// add symbols from source code

	if (itsLanguage != kCBOtherLang)
		{
		CopySymbolsForLanguage(itsLanguage);
		}
}

/******************************************************************************
 CopyWordsFromStyler (protected)

 ******************************************************************************/

void
CBStringCompleter::CopyWordsFromStyler
	(
	CBStylerBase* styler
	)
{
	JStringMapCursor<JFontStyle> cursor(&(styler->GetWordList()));
	while (cursor.Next())
		{
		Add(cursor.GetKey());
		}
}

/******************************************************************************
 CopySymbolsForLanguage (protected)

 ******************************************************************************/

void
CBStringCompleter::CopySymbolsForLanguage
	(
	const CBLanguage lang
	)
{
	JPtrArray<CBProjectDocument>* docList =
		(CBGetDocumentManager())->GetProjectDocList();

	const JSize docCount = docList->GetElementCount();
	for (JIndex i=1; i<=docCount; i++)
		{
		const CBSymbolList* symbolList =
			((docList->GetElement(i))->GetSymbolDirector())->GetSymbolList();

		const JSize symbolCount = symbolList->GetElementCount();
		for (JIndex j=1; j<=symbolCount; j++)
			{
			CBLanguage l;
			CBSymbolList::Type type;
			JBoolean fullyQualifiedFileScope;
			const JString& symbolName =
				symbolList->GetSymbol(j, &l, &type, &fullyQualifiedFileScope);
			if (l == lang && !fullyQualifiedFileScope)
				{
				Add(const_cast<JString*>(&symbolName));
				}
			}
		}
}
