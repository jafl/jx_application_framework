/******************************************************************************
 JXFontNameMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	NameChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for NameNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXFontNameMenu.h"
#include "JXFontNameMenuDirector.h"
#include "JXFontManager.h"
#include "JXDisplay.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryCount = 5;

// JBroadcaster message types

const JUtf8Byte* JXFontNameMenu::kNameNeedsUpdate = "NameNeedsUpdate::JXFontNameMenu";
const JUtf8Byte* JXFontNameMenu::kNameChanged     = "NameChanged::JXFontNameMenu";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFontNameMenu::JXFontNameMenu
	(
	const JString&		title,
	const bool			prependHistory,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h),
	itsBroadcastNameChangeFlag(true),
	itsNameHistory(nullptr)
{
	BuildMenu(prependHistory);
}

JXFontNameMenu::JXFontNameMenu
	(
	const bool		prependHistory,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure),
	itsBroadcastNameChangeFlag(true),
	itsNameHistory(nullptr)
{
	BuildMenu(prependHistory);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontNameMenu::~JXFontNameMenu()
{
	jdelete itsNameHistory;
}

/******************************************************************************
 SetFontName

 ******************************************************************************/

bool
JXFontNameMenu::SetFontName
	(
	const JString& name
	)
{
	bool foundSeparator = itsNameHistory == nullptr;

	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (!foundSeparator && HasSeparatorAfter(i))
		{
			foundSeparator = true;
			continue;
		}
		else if (!foundSeparator)
		{
			continue;
		}

		if (GetItemText(i) == name)
		{
			const JIndex origFontIndex = itsFontIndex;

			itsFontIndex = i;
			SetPopupChoice(itsFontIndex);
			if (itsBroadcastNameChangeFlag && itsFontIndex != origFontIndex)
			{
				Broadcast(NameChanged());
			}

			return true;
		}
	}

	return false;
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
JXFontNameMenu::BuildMenu
	(
	const bool prependHistory
	)
{
	SetHint(JGetString("Hint::JXFontNameMenu"));

	JSize offset = 0;
	if (prependHistory)
	{
		itsNameHistory = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		itsNameHistory->Append(JFontManager::GetDefaultFontName());

		AppendItem(JFontManager::GetDefaultFontName(), kRadioType);
		ShowSeparatorAfter(1);
		offset = 1;
	}

	JXFontManager* fontManager = GetDisplay()->GetXFontManager();

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	fontManager->GetFontNames(&fontNames);

	const JSize count = fontNames.GetItemCount();
	assert( count > 0 );
	JString name, charSet;
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fontName = fontNames.GetItem(i);
		AppendItem(*fontName, kRadioType);
		SetItemFontName(i+offset, *fontName);

		fontManager->Preload(GetItemFont(i+offset).GetID());
	}

	SetUpdateAction(kDisableNone);

	itsFontIndex = 0;
	SetFontName(JFontManager::GetDefaultFontName());
	ListenTo(this);
}

/******************************************************************************
 CreateMenuWindow (virtual protected)

 ******************************************************************************/

JXMenuDirector*
JXFontNameMenu::CreateMenuWindow
	(
	JXWindowDirector* supervisor
	)
{
	return jnew JXFontNameMenuDirector(supervisor, this, GetTextMenuData());
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXFontNameMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		itsBroadcastNameChangeFlag = false;
		UpdateMenu();
		itsBroadcastNameChangeFlag = true;
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		if (itsNameHistory != nullptr)
		{
			const JString& name = GetItemText(selection->GetIndex());
			UpdateHistory(name);
			itsFontIndex = 0;	// force broadcast
			SetFontName(name);
		}
		else
		{
			itsFontIndex = selection->GetIndex();
			Broadcast(NameChanged());
		}
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMenu (private)

 ******************************************************************************/

void
JXFontNameMenu::UpdateMenu()
{
	Broadcast(NameNeedsUpdate());	// before removing history items
	if (itsNameHistory == nullptr)
	{
		CheckItem(itsFontIndex);
		return;
	}

	while (GetItemCount() > 0)
	{
		const bool hadSeparator = HasSeparatorAfter(1);
		RemoveItem(1);
		itsFontIndex--;
		if (hadSeparator)
		{
			break;
		}
	}

	UpdateHistory(GetItemText(itsFontIndex));

	const JSize count = itsNameHistory->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString* s = itsNameHistory->GetItem(i);
		InsertItem(i, *s, kRadioType);
		SetItemFontName(i, *s);
		itsFontIndex++;
	}

	ShowSeparatorAfter(count);

	CheckItem(1);
	CheckItem(itsFontIndex);
}

/******************************************************************************
 UpdateHistory (private)

 ******************************************************************************/

void
JXFontNameMenu::UpdateHistory
	(
	const JString& name
	)
{
	assert( itsNameHistory != nullptr );

	for (JIndex i=1; i<=itsNameHistory->GetItemCount(); i++)
	{
		if (name == *itsNameHistory->GetItem(i))
		{
			itsNameHistory->MoveItemToIndex(i, 1);
			return;
		}
	}

	itsNameHistory->InsertAtIndex(1, name);

	while (itsNameHistory->GetItemCount() > kHistoryCount)
	{
		itsNameHistory->DeleteItem(itsNameHistory->GetItemCount());
	}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXFontNameMenu::SetToPopupChoice
	(
	const bool isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsFontIndex);
}
