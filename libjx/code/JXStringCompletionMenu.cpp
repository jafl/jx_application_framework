/******************************************************************************
 JXStringCompletionMenu.cpp

	This menu is an action menu, so all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXStringCompletionMenu.h"
#include "JXTEBase.h"
#include "JXFontManager.h"
#include "jXConstants.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JSize kMaxItemCount = 100;

const JSize kSpecialCmdCount = 1;
const JIndex kInsertTabCmd   = 1;

static const JUtf8Byte* kItemPrefixStr        = "    ";
static const JUtf8Byte* kShortcutSeparatorStr = " - ";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringCompletionMenu::JXStringCompletionMenu
	(
	JXTEBase*		te,
	const bool	allowTabChar
	)
	:
	JXTextMenu("*", te, kFixedLeft, kFixedTop, 0,0, 10,10),
	itsAllowTabChar(allowTabChar)
{
	itsTE              = te;
	itsRequestCount    = 0;
	itsPrefixCharCount = 0;

	Hide();
	SetToHiddenPopupMenu(true);
	CompressHeight();
	SetUpdateAction(kDisableNone);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStringCompletionMenu::~JXStringCompletionMenu()
{
}

/******************************************************************************
 AddString

	Returns false if it doesn't want any more strings.

 ******************************************************************************/

bool
JXStringCompletionMenu::AddString
	(
	const JString& str
	)
{
	const JIndex i = GetItemCount()+1;
	if (i <= kMaxItemCount)
	{
		if (IsEmpty())
		{
			SetDefaultFont(itsTE->GetText()->GetDefaultFont(), false);
		}

		JString shortcut;
		if (i <= 10)
		{
			shortcut = JString((JUInt64) i%10);
		}
		else if (i <= 36)
		{
			const JUtf8Byte s[] = { char('a' + i - 11), 0 };
			shortcut.Set(s);
		}

		JString s;
		if (!shortcut.IsEmpty())
		{
			s.Append(shortcut);
			s.Append(kShortcutSeparatorStr);
		}
		else
		{
			s.Append(kItemPrefixStr);
		}
		s += str;

		AppendItem(s, kPlainType, shortcut);
		return true;
	}
	else if (i == kMaxItemCount+1)
	{
		AppendItem(JGetString("TruncationMarker::JXStringCompletionMenu"));

		JFont font = JFontManager::GetDefaultFont();
		font.SetItalic(true);
		SetItemFont(i, font);
		return false;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 CompletionRequested

 ******************************************************************************/

void
JXStringCompletionMenu::CompletionRequested
	(
	const JSize prefixCharCount
	)
{
	itsRequestCount++;
	if (itsRequestCount > 1 && !IsEmpty())
	{
		itsPrefixCharCount = prefixCharCount;

		// prepend "insert tab character" option

		JString s;
		if (itsAllowTabChar &&
			(!GetItemShortcuts(1, &s) || s != JGetString("InsertTabShortcut::JXStringCompletionMenu")))
		{
			PrependItem(
				JGetString("InsertTabText::JXStringCompletionMenu"),
				kPlainType,
				JGetString("InsertTabShortcut::JXStringCompletionMenu"));
		}

		// place it next to the caret (use the character in front of the caret)

		const JIndex charIndex = itsTE->GetInsertionCharIndex() - 1;
		const JCoordinate x    = itsTE->GetCharRight(charIndex) + 2;
		const JIndex lineIndex = itsTE->GetLineForChar(charIndex);
		const JCoordinate y    = itsTE->GetLineTop(lineIndex) - GetFrameHeight();
		Place(x,y);

		// open it

		HandleShortcut('\t', JXKeyModifiers(GetDisplay()));
	}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXStringCompletionMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSelection(selection->GetIndex());
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 HandleSelection (private)

 ******************************************************************************/

void
JXStringCompletionMenu::HandleSelection
	(
	const JIndex index
	)
{
	if (itsAllowTabChar && index == kInsertTabCmd)
	{
		// tab may insert spaces - don't trigger derived class, to avoid infinite loop
		itsTE->JXTEBase::HandleKeyPress(JUtf8Character(kJTabKey), 0, JXKeyModifiers(GetDisplay()));
	}
	else if (index - (itsAllowTabChar ? kSpecialCmdCount : 0) <= kMaxItemCount)
	{
		JString s = GetItemText(index);
		JStringIterator iter(&s);
		iter.SkipNext(strlen(kItemPrefixStr) + itsPrefixCharCount);
		iter.RemoveAllPrev();
		iter.Invalidate();	// avoid double iterator
		itsTE->Paste(s);
	}
}
