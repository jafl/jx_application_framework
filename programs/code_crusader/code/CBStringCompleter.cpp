/******************************************************************************
 CBStringCompleter.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBStringCompleter.h"
#include "CBStylerBase.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBSymbolList.h"
#include "cbGlobals.h"
#include <JXStringCompletionMenu.h>
#include <JTextEditor.h>
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStringCompleter::CBStringCompleter
	(
	const CBLanguage	lang,
	const JSize			keywordCount,
	const JUtf8Byte**	keywordList,
	const JString::Case	caseSensitive
	)
	:
	itsLanguage(lang),
	itsPredefKeywordCount(keywordCount),
	itsPrefefKeywordList(keywordList),
	itsCaseSensitiveFlag(caseSensitive)
{
	itsStringList = jnew JPtrArray<JString>(JPtrArrayT::kForgetAll, 4096);
	assert( itsStringList != nullptr );
	itsStringList->SetSortOrder(JListT::kSortAscending);

	itsStringList->SetCompareFunction(
		itsCaseSensitiveFlag ? JCompareStringsCaseSensitive : JCompareStringsCaseInsensitive);

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
		CBGetDocumentManager()->GetProjectDocList();

	for (auto* doc : *docList)
		{
		ListenTo(doc->GetSymbolDirector()->GetSymbolList());
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
		Add(JString(itsPrefefKeywordList[i], JString::kNoCopy));
		}
}

/******************************************************************************
 Add

 ******************************************************************************/

void
CBStringCompleter::Add
	(
	const JString& str
	)
{
	auto* s = jnew JString(str);
	assert( s != nullptr );
	if (itsStringList->InsertSorted(s, false))
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
	itsStringList->InsertSorted(s, false);
}

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

	Returns true if the text behind the caret was completed.

 ******************************************************************************/

bool
CBStringCompleter::Complete
	(
	JTextEditor*			te,
	JXStringCompletionMenu*	menu
	)
{
	return Complete(te, true, menu);
}

// private

bool
CBStringCompleter::Complete
	(
	JTextEditor*			te,
	const bool			includeNS,
	JXStringCompletionMenu*	menu
	)
{
	assert( menu != nullptr );

	JIndex caretIndex;
	if (!te->GetCaretLocation(&caretIndex))
		{
		return false;
		}

	const JString& text = te->GetText()->GetText();

	JStringIterator iter(text, kJIteratorStartBefore, caretIndex);
	iter.BeginMatch();
	JUtf8Character c;
	while (iter.Prev(&c, kJIteratorStay) && IsWordCharacter(c, includeNS))
		{
		iter.SkipPrev();
		}

	if ((iter.AtEnd() && caretIndex > text.GetCharacterCount()) ||
		iter.GetNextCharacterIndex() == caretIndex)
		{
		return false;
		}

	const JStringMatch m    = iter.FinishMatch();	// must exist after invalidating iterator
	const JSize matchLength = m.GetCharacterRange().GetCount();

	iter.Invalidate();

	JString s;
	const JSize matchCount = Complete(m.GetString(), &s, menu);
	if (matchCount > 0 &&
		s.GetCharacterCount() > matchLength)
		{
		te->Paste(JString(s.GetBytes() + m.GetUtf8ByteRange().GetCount(), JString::kNoCopy));
		menu->ClearRequestCount();
		return true;
		}
	else if (matchCount > 1)
		{
		menu->CompletionRequested(matchLength);
		return true;
		}
	else if (matchCount == 0 && includeNS)
		{
		// try once more without namespace
		return Complete(te, false, menu);
		}

	return false;
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

	bool found;
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
	bool addString = true;
	for (JIndex i=startIndex; i<=stringCount; i++)
		{
		const JString* s = itsStringList->GetElement(i);
		if (!s->BeginsWith(prefix, itsCaseSensitiveFlag))
			{
			break;
			}

		if (matchCount > 0)
			{
			const JSize matchLength  = JString::CalcCharacterMatchLength(*maxPrefix, *s, itsCaseSensitiveFlag);
			const JSize prefixLength = maxPrefix->GetCharacterCount();
			if (matchLength < prefixLength)
				{
				JStringIterator iter(maxPrefix, kJIteratorStartAtEnd);
				iter.RemovePrev(prefixLength - matchLength);
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
		const auto* info =
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
	JStringMapCursor<JFontStyle> cursor(&styler->GetWordList());
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
		CBGetDocumentManager()->GetProjectDocList();

	for (auto* doc : *docList)
		{
		const CBSymbolList* symbolList = doc->GetSymbolDirector()->GetSymbolList();

		const JSize symbolCount = symbolList->GetElementCount();
		for (JIndex j=1; j<=symbolCount; j++)
			{
			CBLanguage l;
			CBSymbolList::Type type;
			bool fullyQualifiedFileScope;
			const JString& symbolName =
				symbolList->GetSymbol(j, &l, &type, &fullyQualifiedFileScope);
			if (l == lang && !fullyQualifiedFileScope)
				{
				Add(const_cast<JString*>(&symbolName));
				}
			}
		}
}
