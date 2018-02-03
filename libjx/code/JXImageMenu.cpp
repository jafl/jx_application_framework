/******************************************************************************
 JXImageMenu.cpp

	A menu that displays an image for each menu item.

	BASE CLASS = JXMenu

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXImageMenu.h>
#include <JXImageMenuData.h>
#include <JXImageMenuDirector.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMenu::JXImageMenu
	(
	const JString&		title,
	const JSize			columnCount,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXImageMenuX(columnCount);
}

JXImageMenu::JXImageMenu
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const JSize			columnCount,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXMenu(image, menuOwnsImage, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXImageMenuX(columnCount);
}

JXImageMenu::JXImageMenu
	(
	const JSize		columnCount,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXMenu(owner, itemIndex, enclosure)
{
	JXImageMenuX(columnCount);
}

// private

void
JXImageMenu::JXImageMenuX
	(
	const JSize columnCount
	)
{
	itsIconMenuData = jnew JXImageMenuData(columnCount);
	assert( itsIconMenuData != NULL );

	SetBaseItemData(itsIconMenuData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageMenu::~JXImageMenu()
{
	ClearBaseItemData();
	jdelete itsIconMenuData;
}

/******************************************************************************
 New item

 ******************************************************************************/

void
JXImageMenu::InsertItem
	(
	const JIndex		index,
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const ItemType		type,
	const JString&		id
	)
{
	itsIconMenuData->InsertItem(index, image, menuOwnsImage, type, id);
}

void
JXImageMenu::PrependItem
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const ItemType		type,
	const JString&		id
	)
{
	itsIconMenuData->PrependItem(image, menuOwnsImage, type, id);
}

void
JXImageMenu::AppendItem
	(
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const ItemType		type,
	const JString&		id
	)
{
	itsIconMenuData->AppendItem(image, menuOwnsImage, type, id);
}

/******************************************************************************
 Item image

 ******************************************************************************/

const JXImage*
JXImageMenu::GetItemImage
	(
	const JIndex index
	)
	const
{
	return itsIconMenuData->GetImage(index);
}

void
JXImageMenu::SetItemImage
	(
	const JIndex	index,
	const JXPM&		data
	)
{
	JXImage* image = jnew JXImage(GetDisplay(), data);
	assert( image != NULL );
	SetItemImage(index, image, kJTrue);
}

void
JXImageMenu::SetItemImage
	(
	const JIndex	index,
	JXImage*		image,
	const JBoolean	menuOwnsImage
	)
{
	itsIconMenuData->SetImage(index, image, menuOwnsImage);
}

/******************************************************************************
 CreateMenuWindow (virtual protected)

 ******************************************************************************/

JXMenuDirector*
JXImageMenu::CreateMenuWindow
	(
	JXWindowDirector* supervisor
	)
{
	JXImageMenuDirector* dir =
		jnew JXImageMenuDirector(supervisor, this, itsIconMenuData);
	assert( dir != NULL );
	return dir;
}

/******************************************************************************
 AdjustPopupChoiceTitle (virtual protected)

 ******************************************************************************/

void
JXImageMenu::AdjustPopupChoiceTitle
	(
	const JIndex index
	)
{
	const JXImage* titleImage = NULL;
	if (GetTitleImage(&titleImage))
		{
		JXImage* image = const_cast<JXImage*>(GetItemImage(index));
		SetTitle(NULL, image, kJFalse);
		}
}
