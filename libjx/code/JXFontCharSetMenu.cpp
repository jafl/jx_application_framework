/******************************************************************************
 JXFontCharSetMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	CharSetChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for CharSetNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright © 1996-2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFontCharSetMenu.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jXConstants.h>
#include <jXActionDefs.h>
#include <JFontManager.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXFontCharSetMenu::kCharSetNeedsUpdate = "CharSetNeedsUpdate::JXFontCharSetMenu";
const JCharacter* JXFontCharSetMenu::kCharSetChanged     = "CharSetChanged::JXFontCharSetMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontCharSetMenu::JXFontCharSetMenu
	(
	const JCharacter*	fontName,
	const JSize			fontSize,
	const JCharacter*	title,
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
	JXFontCharSetMenuX(NULL, NULL);
	BuildMenu(fontName, fontSize);
}

JXFontCharSetMenu::JXFontCharSetMenu
	(
	const JCharacter*	fontName,
	const JSize			fontSize,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXFontCharSetMenuX(NULL, NULL);
	BuildMenu(fontName, fontSize);
}

JXFontCharSetMenu::JXFontCharSetMenu
	(
	JXFontNameMenu*		fontMenu,
	JXFontSizeMenu*		sizeMenu,
	const JCharacter*	title, 
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
	JXFontCharSetMenuX(fontMenu, sizeMenu);
	const JString fontName = itsFontNameMenu->GetFontName();
	BuildMenu(fontName, itsFontSizeMenu->GetFontSize());
}

JXFontCharSetMenu::JXFontCharSetMenu
	(
	JXFontNameMenu*		fontMenu,
	JXFontSizeMenu*		sizeMenu,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXFontCharSetMenuX(fontMenu, sizeMenu);
	const JString fontName = itsFontNameMenu->GetFontName();
	BuildMenu(fontName, itsFontSizeMenu->GetFontSize());
}

// private

void
JXFontCharSetMenu::JXFontCharSetMenuX
	(
	JXFontNameMenu*	fontMenu,
	JXFontSizeMenu*	sizeMenu
	)
{
	assert( (fontMenu == NULL && sizeMenu == NULL) ||
			(fontMenu != NULL && sizeMenu != NULL) );

	itsFontNameMenu = fontMenu;
	if (itsFontNameMenu != NULL)
		{
		ListenTo(itsFontNameMenu);
		}

	itsFontSizeMenu = sizeMenu;
	if (itsFontSizeMenu != NULL)
		{
		ListenTo(itsFontSizeMenu);
		}

	itsBroadcastChangeFlag = kJTrue;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontCharSetMenu::~JXFontCharSetMenu()
{
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXFontCharSetMenu::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JString fontName, charSet;
	JFontManager::ExtractCharacterSet(name, &fontName, &charSet);
	if (charSet.IsEmpty())
		{
		charSet = GetCharSet();
		}
	BuildMenu(name, size);
	SetCharSet(charSet);
}

/******************************************************************************
 SetCharSet

 ******************************************************************************/

JBoolean
JXFontCharSetMenu::SetCharSet
	(
	const JCharacter* charSet
	)
{
	JIndex newIndex = 0;
	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetItemText(i) == charSet)
			{
			newIndex = i;
			break;
			}
		}

	if (newIndex > 0)
		{
		itsCurrIndex = newIndex;
		itsCharSet   = charSet;

		SetPopupChoice(itsCurrIndex);
		if (itsBroadcastChangeFlag)
			{
			Broadcast(CharSetChanged(itsCharSet));
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SelectDefaultCharSet

 ******************************************************************************/

void
JXFontCharSetMenu::SelectDefaultCharSet()
{
	JString charSet;
	if (!JXGetCharacterSetName(&charSet) ||
		!SetCharSet(charSet))
		{
		SetCharSet("iso8859-1");
		}
}

/******************************************************************************
 BuildMenu (private)

	Caller should call SetCharSet() afterwards.

 ******************************************************************************/

void
JXFontCharSetMenu::BuildMenu
	(
	const JCharacter*	fontName,
	const JSize			fontSize
	)
{
	RemoveAllItems();

	JPtrArray<JString> charSetList(JPtrArrayT::kDeleteAll);
	if (!(GetFontManager())->GetFontCharSets(fontName, fontSize, &charSetList))
		{
		itsCurrIndex = 0;
		itsCharSet.Clear();
		StopListening(this);
		return;
		}

	const JSize count = charSetList.GetElementCount();
	JString id;
	for (JIndex i=1; i<=count; i++)
		{
		const JString* charSet = charSetList.NthElement(i);
		id                     = *charSet + "::JX";
		AppendItem(*charSet, kJTrue, kJTrue, NULL, NULL, id);
		}

	SetUpdateAction(kDisableNone);

	itsCurrIndex = 1;
	itsCharSet   = *(charSetList.FirstElement());

	StopListening(this);
	JString name, charSet;
	if (JFontManager::ExtractCharacterSet(fontName, &name, &charSet))
		{
		SetCharSet(charSet);
		}
	else
		{
		SelectDefaultCharSet();
		}
	ListenTo(this);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFontCharSetMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == itsFontNameMenu && message.Is(JXFontNameMenu::kNameChanged)) ||
		(sender == itsFontSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged)))
		{
		itsBroadcastChangeFlag = kJFalse;
		const JString charSet  = GetCharSet();
		const JString fontName = itsFontNameMenu->GetFontName();
		BuildMenu(fontName, itsFontSizeMenu->GetFontSize());
		SetCharSet(charSet);
		itsBroadcastChangeFlag = kJTrue;
		}

	else if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsBroadcastChangeFlag = kJFalse;
		Broadcast(CharSetNeedsUpdate());
		CheckItem(itsCurrIndex);
		itsBroadcastChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		ChooseCharSet(selection->GetIndex());
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 ChooseCharSet (private)

 ******************************************************************************/

void
JXFontCharSetMenu::ChooseCharSet
	(
	const JIndex index
	)
{
	itsCurrIndex = index;
	itsCharSet   = GetItemText(index);
	Broadcast(CharSetChanged(itsCharSet));
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
JXFontCharSetMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsCurrIndex);
}
