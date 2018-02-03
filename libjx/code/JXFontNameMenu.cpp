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

#include <JXFontNameMenu.h>
#include <jXGlobals.h>
#include <JFontManager.h>
#include <jAssert.h>

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
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsBroadcastNameChangeFlag = kJTrue;
	BuildMenu();
}

JXFontNameMenu::JXFontNameMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	itsBroadcastNameChangeFlag = kJTrue;
	BuildMenu();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontNameMenu::~JXFontNameMenu()
{
}

/******************************************************************************
 SetFontName

 ******************************************************************************/

JBoolean
JXFontNameMenu::SetFontName
	(
	const JString& name
	)
{
	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetItemText(i) == name)
			{
			const JIndex origFontIndex = itsFontIndex;

			itsFontIndex = i;
			SetPopupChoice(itsFontIndex);
			if (itsBroadcastNameChangeFlag && itsFontIndex != origFontIndex)
				{
				Broadcast(NameChanged());
				}

			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
JXFontNameMenu::BuildMenu()
{
	RemoveAllItems();

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	GetFontManager()->GetFontNames(&fontNames);

	const JSize count = fontNames.GetElementCount();
	assert( count > 0 );
	JString name, charSet;
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fontName = fontNames.GetElement(i);
		AppendItem(*fontName, kRadioType);
		SetItemFontName(i, *fontName);
		}

	SetUpdateAction(kDisableNone);

	itsFontIndex = 1;
	SetFontName(JGetDefaultFontName());
	ListenTo(this);
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
		itsBroadcastNameChangeFlag = kJFalse;
		Broadcast(NameNeedsUpdate());
		CheckItem(itsFontIndex);
		itsBroadcastNameChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsFontIndex = selection->GetIndex();
		Broadcast(NameChanged());
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXFontNameMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsFontIndex);
}
