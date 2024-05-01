/******************************************************************************
 JXXFontMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	FontChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for FontNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXFontNameMenu

	Copyright (C) 1996-2010 by John Lindal.

 ******************************************************************************/

#include "JXXFontMenu.h"
#include "JXFontManager.h"
#include "JXDisplay.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about not finding any fonts to match the given regex.

 ******************************************************************************/

bool
JXXFontMenu::Create
	(
	const JRegex&		regex,

	const std::function<std::weak_ordering(JString * const &, JString * const &)> compare,

	const JString&		title,
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
	*menu = nullptr;

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	enclosure->GetXFontManager()->GetXFontNames(regex, &fontNames, &compare);
	if (!fontNames.IsEmpty())
	{
		*menu = jnew JXXFontMenu(fontNames, title, enclosure, hSizing, vSizing, x,y, w,h);
	}

	return *menu != nullptr;
}

bool
JXXFontMenu::Create
	(
	const JRegex&		regex,

	const std::function<std::weak_ordering(JString * const &, JString * const &)> compare,

	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure,
	JXXFontMenu**		menu
	)
{
	*menu = nullptr;

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	enclosure->GetXFontManager()->GetXFontNames(regex, &fontNames, &compare);
	if (!fontNames.IsEmpty())
	{
		*menu = jnew JXXFontMenu(fontNames, owner, itemIndex, enclosure);
	}

	return *menu != nullptr;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXXFontMenu::JXXFontMenu
	(
	const JPtrArray<JString>&	fontNames,
	const JString&				title,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXFontNameMenu(title, false, enclosure, hSizing, vSizing, x,y, w,h)
{
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
	JXFontNameMenu(false, owner, itemIndex, enclosure)
{
	BuildMenu(fontNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXXFontMenu::~JXXFontMenu()
{
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

	JXFontManager* fontManager = GetDisplay()->GetXFontManager();

	const JSize count = fontNames.GetItemCount();
	assert( count > 0 );
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fontName = fontNames.GetItem(i);
		AppendItem(*fontName, kRadioType);
		SetItemFontName(i, *fontName);

		fontManager->Preload(GetItemFont(i).GetID());
	}

	SetFontName(*fontNames.GetFirstItem());
}
