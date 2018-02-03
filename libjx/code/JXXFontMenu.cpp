/******************************************************************************
 JXXFontMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	FontChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should listen
	for FontNeedsUpdate instead of NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996-2010 by John Lindal.

 ******************************************************************************/

#include <JXXFontMenu.h>
#include <JXFontManager.h>
#include <jAssert.h>

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
	*menu = NULL;

	JPtrArray<JString> fontNames(JPtrArrayT::kDeleteAll);
	(enclosure->GetXFontManager())->GetXFontNames(regex, &fontNames, compare);
	if (!fontNames.IsEmpty())
		{
		*menu = jnew JXXFontMenu(fontNames, title, enclosure, hSizing, vSizing, x,y, w,h);
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
		*menu = jnew JXXFontMenu(fontNames, owner, itemIndex, enclosure);
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
	JXFontNameMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
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
	JXFontNameMenu(owner, itemIndex, enclosure)
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

	const JSize count = fontNames.GetElementCount();
	assert( count > 0 );
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fontName = fontNames.GetElement(i);
		AppendItem(*fontName, kRadioType);
		SetItemFontName(i, *fontName);
		}

	SetFontName(*(fontNames.GetFirstElement()));
}
