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

#include "jx-af/jx/JXFontNameMenu.h"
#include "jx-af/jx/JXFontNameMenuDirector.h"
#include "jx-af/jx/JXFontManager.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/jXGlobals.h"
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
	itsBroadcastNameChangeFlag(true)
{
	BuildMenu();
}

JXFontNameMenu::JXFontNameMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure),
	itsBroadcastNameChangeFlag(true)
{
	BuildMenu();
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
	bool foundSeparator = false;

	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (HasSeparatorAfter(i))
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
JXFontNameMenu::BuildMenu()
{
	SetHint(JGetString("Hint::JXFontNameMenu"));

	itsNameHistory = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNameHistory != nullptr );
	itsNameHistory->Append(JFontManager::GetDefaultFontName());

	AppendItem(JFontManager::GetDefaultFontName(), kRadioType);
	ShowSeparatorAfter(1);

	JXFontManager* fontManager = GetDisplay()->GetXFontManager();

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	fontManager->GetFontNames(&fontNames);

	const JSize count = fontNames.GetElementCount();
	assert( count > 0 );
	JString name, charSet;
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fontName = fontNames.GetElement(i);
		AppendItem(*fontName, kRadioType);
		SetItemFontName(i+1, *fontName);

		fontManager->Preload(GetItemFont(i+1).GetID());
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
	auto* dir =
		jnew JXFontNameMenuDirector(supervisor, this, GetTextMenuData());
	assert( dir != nullptr );
	return dir;
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		const JString& name = GetItemText(selection->GetIndex());
		UpdateHistory(name);
		itsFontIndex = 0;	// force broadcast
		SetFontName(name);
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

	const JSize count = itsNameHistory->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString* s = itsNameHistory->GetElement(i);
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
	for (JIndex i=1; i<=itsNameHistory->GetElementCount(); i++)
	{
		if (name == *itsNameHistory->GetElement(i))
		{
			itsNameHistory->MoveElementToIndex(i, 1);
			return;
		}
	}

	itsNameHistory->InsertAtIndex(1, name);

	while (itsNameHistory->GetElementCount() > kHistoryCount)
	{
		itsNameHistory->DeleteElement(itsNameHistory->GetElementCount());
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
