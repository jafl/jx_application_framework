/******************************************************************************
 JXStringCompletionMenu.cpp

	This menu is an action menu, so all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JXStringCompletionMenu.h>
#include <JXTEBase.h>
#include <JXFontManager.h>
#include <jXConstants.h>
#include <JStringIterator.h>
#include <jGlobals.h>
#include <jAssert.h>

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
	const JBoolean	allowTabChar
	)
	:
	JXTextMenu(JString("*", 0, kJFalse), te, kFixedLeft, kFixedTop, 0,0, 10,10),
	itsAllowTabChar(allowTabChar)
{
	itsTE           = te;
	itsRequestCount = 0;
	itsPrefixLength = 0;

	Hide();
	SetToHiddenPopupMenu(kJTrue);
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

	Returns kJFalse if it doesn't want any more strings.

 ******************************************************************************/

JBoolean
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
			SetDefaultFont(itsTE->GetDefaultFont(), kJFalse);
			}

		JString shortcut;
		if (i <= 10)
			{
			shortcut = JString(i%10, JString::kBase10);
			}
		else if (i <= 36)
			{
			const JUtf8Byte s[] = { 'a' + i - 11, 0 };
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
		return kJTrue;
		}
	else if (i == kMaxItemCount+1)
		{
		AppendItem(JGetString("TruncationMarker::JXStringCompletionMenu"));

		JFont font = GetFontManager()->GetDefaultFont();
		font.SetItalic(kJTrue);
		SetItemFont(i, font);
		return kJFalse;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CompletionRequested

 ******************************************************************************/

void
JXStringCompletionMenu::CompletionRequested
	(
	const JSize prefixLength
	)
{
	itsRequestCount++;
	if (itsRequestCount > 1 && !IsEmpty())
		{
		itsPrefixLength = prefixLength;

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

		const JIndex charIndex = itsTE->GetInsertionIndex() - 1;
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
		// tab may insert spaces
		itsTE->JXTEBase::HandleKeyPress('\t', JXKeyModifiers(GetDisplay()));
		}
	else if (index - (itsAllowTabChar ? kSpecialCmdCount : 0) <= kMaxItemCount)
		{
		JString s = GetItemText(index);
		JStringIterator iter(&s);
		iter.SkipNext(strlen(kItemPrefixStr));
		iter.RemoveAllPrev();
		itsTE->Paste(s);
		}
}
