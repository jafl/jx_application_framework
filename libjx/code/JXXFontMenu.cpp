/******************************************************************************
 JXXFontMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	FontChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for FontNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXXFontMenu.h>
#include <JXFontManager.h>
#include <JString.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXXFontMenu::kFontNeedsUpdate = "FontNeedsUpdate::JXXFontMenu";
const JCharacter* JXXFontMenu::kFontChanged     = "FontChanged::JXXFontMenu";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about not finding any fonts to match the given regex.

 ******************************************************************************/

JBoolean
JXXFontMenu::Create
	(
	const JRegex&		regex,
	JSortXFontNamesFn	compare,
	const JCharacter*	title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	JXXFontMenu**		menu
	)
{
	*menu = NULL;

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	(enclosure->GetXFontManager())->GetXFontNames(regex, &fontNames, compare);
	if (!fontNames.IsEmpty())
		{
		*menu = new JXXFontMenu(fontNames, title, enclosure, hSizing, vSizing, x,y, w,h);
		assert( *menu != NULL );
		}

	return JConvertToBoolean( *menu != NULL );
}

JBoolean
JXXFontMenu::Create
	(
	const JRegex&		regex,
	JSortXFontNamesFn	compare,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure,
	JXXFontMenu**		menu
	)
{
	*menu = NULL;

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	(enclosure->GetXFontManager())->GetXFontNames(regex, &fontNames, compare);
	if (!fontNames.IsEmpty())
		{
		*menu = new JXXFontMenu(fontNames, owner, itemIndex, enclosure);
		assert( *menu != NULL );
		}

	return JConvertToBoolean( *menu != NULL );
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXXFontMenu::JXXFontMenu
	(
	const JPtrArray<JString>&	fontNames,
	const JCharacter*			title,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsBroadcastFontChangeFlag = kJTrue;
	BuildMenu(fontNames);
}

JXXFontMenu::JXXFontMenu
	(
	const JPtrArray<JString>&	fontNames,
	JXMenu*						owner,
	const JIndex				itemIndex,
	JXContainer*				enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	itsBroadcastFontChangeFlag = kJTrue;
	BuildMenu(fontNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXXFontMenu::~JXXFontMenu()
{
}

/******************************************************************************
 GetFontID

 ******************************************************************************/

JFontID
JXXFontMenu::GetFontID()
	const
{
	const JXFontManager* fontMgr = GetXFontManager();
	const JString fontName       = GetFontName();

	JFontID id;
	const JBoolean ok = fontMgr->GetFontID(fontName, &id);
	assert( ok );

	return id;
}

/******************************************************************************
 GetFontName (private)

 ******************************************************************************/

JString
JXXFontMenu::GetFontName
	(
	const JIndex index
	)
	const
{
	JString name = GetItemText(index);

	JString charSet;
	if (GetItemNMShortcut(index, &charSet))
		{
		charSet.TrimWhitespace();
		name = JFontManager::CombineNameAndCharacterSet(name, charSet);
		}

	return name;
}

/******************************************************************************
 SetFontName

 ******************************************************************************/

void
JXXFontMenu::SetFontName
	(
	const JCharacter* name
	)
{
JIndex i;

	const JIndex count = GetItemCount();
	for (i=1; i<=count; i++)
		{
		if (GetFontName(i) == name)
			{
			SetFontName1(i);
			return;
			}
		}

	// catch the case where they don't specify the char set

	for (i=1; i<=count; i++)
		{
		if (GetItemText(i) == name)
			{
			SetFontName1(i);
			return;
			}
		}
}

// private

void
JXXFontMenu::SetFontName1
	(
	const JIndex index
	)
{
	const JIndex origFontIndex = itsFontIndex;

	itsFontIndex = index;
	SetPopupChoice(itsFontIndex);
	if (itsBroadcastFontChangeFlag && itsFontIndex != origFontIndex)
		{
		Broadcast(FontChanged());
		}
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
JXXFontMenu::BuildMenu
	(
	const JPtrArray<JString>& fontNames
	)
{
	RemoveAllItems();

	const JSize count = fontNames.GetElementCount();
	assert( count > 0 );
	for (JIndex i=1; i<=count; i++)
		{
		AppendFontItem(*(fontNames.NthElement(i)), kJTrue);
		}

	SetUpdateAction(kDisableNone);

	itsFontIndex = 1;
	SetFontName(*(fontNames.FirstElement()));
	ListenTo(this);
}

/******************************************************************************
 AppendFontItem

 ******************************************************************************/

void
JXXFontMenu::AppendFontItem
	(
	const JCharacter*	fontName,
	const JBoolean		isRawXFontName
	)
{
	JString name, charSet;
	JBoolean hasCharSet = kJFalse;
	if (isRawXFontName)
		{
		name = fontName;
		}
	else
		{
		hasCharSet = JFontManager::ExtractCharacterSet(fontName, &name, &charSet);
		}

	AppendItem(name, kJTrue, kJTrue);
	if (hasCharSet)
		{
		charSet.PrependCharacter(' ');
		charSet.AppendCharacter(' ');
		SetItemNMShortcut(GetItemCount(), charSet);
		}

	if (isRawXFontName)
		{
		JFontID id;
		const JBoolean ok = (GetXFontManager())->GetFontID(fontName, &id);
		assert( ok );
		SetItemFont(GetItemCount(), id, 10, JFontStyle());	// size and style are irrelevant
		}
	else
		{
		SetItemFontName(GetItemCount(), fontName);
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXXFontMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsBroadcastFontChangeFlag = kJFalse;
		Broadcast(FontNeedsUpdate());
		CheckItem(itsFontIndex);
		itsBroadcastFontChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsFontIndex = selection->GetIndex();
		Broadcast(FontChanged());
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
JXXFontMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsFontIndex);
}
