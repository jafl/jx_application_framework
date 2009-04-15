/******************************************************************************
 JXStringCompletionMenu.cpp

	This menu is an action menu, so all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStringCompletionMenu.h>
#include <JXTEBase.h>
#include <jXConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kMaxItemCount               = 100;
static const JCharacter* kTruncationStr = "etc.";

const JSize kSpecialCmdCount                = 1;
const JIndex kInsertTabCmd                  = 1;
static const JCharacter* kInsertTabStr      = "` - tab character";
static const JCharacter* kInsertTabShortcut = "`";

static const JCharacter* kItemPrefixStr = "    ";
const JSize kItemPrefixLength           = strlen(kItemPrefixStr);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringCompletionMenu::JXStringCompletionMenu
	(
	JXTEBase*		te,
	const JBoolean	allowTabChar
	)
	:
	JXTextMenu("*", te, kFixedLeft, kFixedTop, 0,0, 10,10),
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
	const JCharacter* str
	)
{
	const JIndex i = GetItemCount()+1;
	if (i <= kMaxItemCount)
		{
		if (IsEmpty())
			{
			JFontID id;
			JSize size;
			JFontStyle style;
			itsTE->GetDefaultFont(&id, &size, &style);
			SetDefaultFont(id, size, style, kJFalse);
			}

		JString shortcut;
		if (i <= 10)
			{
			shortcut = JString(i%10, 0);
			}
		else if (i <= 36)
			{
			shortcut = " ";
			shortcut.SetCharacter(1, 'a' + i - 11);
			}

		JString s = kItemPrefixStr;
		s += str;
		if (!shortcut.IsEmpty())
			{
			s.SetCharacter(1, shortcut.GetFirstCharacter());
			s.SetCharacter(3, '-');
			}

		AppendItem(s, kJFalse, kJFalse, shortcut);
		return kJTrue;
		}
	else if (i == kMaxItemCount+1)
		{
		AppendItem(kTruncationStr);
		SetItemFont(i, JGetDefaultFontName(), kJXDefaultFontSize,
					JFontStyle(kJFalse, kJTrue, 0, kJFalse));
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
			(!GetItemShortcuts(1, &s) || s != kInsertTabShortcut))
			{
			PrependItem(kInsertTabStr, kJFalse, kJFalse, kInsertTabShortcut);
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
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
		s.RemoveSubstring(1, kItemPrefixLength + itsPrefixLength);
		itsTE->Paste(s);
		}
}
