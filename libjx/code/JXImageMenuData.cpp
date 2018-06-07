/******************************************************************************
 JXImageMenuData.cpp

	Stores an image for each menu item.

	BASE CLASS = JXMenuData

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXImageMenuData.h>
#include <JXImageMenuTable.h>
#include <JXImage.h>
#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kMinCellSize = 14;	// 10 + 2+2

// JBroadcaster message types

const JUtf8Byte* JXImageMenuData::kImageChanged  = "ImageChanged::JXImageMenuData";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMenuData::JXImageMenuData
	(
	const JSize columnCount
	)
	:
	JXMenuData()
{
	itsIconData = jnew JArray<IconData>;
	assert( itsIconData != nullptr );

	itsColumnCount = columnCount;

	itsNeedGeomRecalcFlag = kJTrue;
	itsRowHeight          = 1;
	itsColWidth           = 1;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageMenuData::~JXImageMenuData()
{
	DeleteAll();
	jdelete itsIconData;
}

/******************************************************************************
 InsertItem

 ******************************************************************************/

void
JXImageMenuData::InsertItem
	(
	const JIndex			index,
	JXImage*				image,
	const JBoolean			menuOwnsImage,
	const JXMenu::ItemType	type,
	const JString&			id
	)
{
	assert( image != nullptr );

	const IconData itemData(image, menuOwnsImage);
	itsIconData->InsertElementAtIndex(index, itemData);

	JXMenuData::InsertItem(index, type, JString::empty, id);

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 DeleteItem (virtual)

 ******************************************************************************/

void
JXImageMenuData::DeleteItem
	(
	const JIndex index
	)
{
	IconData itemData = itsIconData->GetElement(index);
	CleanOutIconItem(&itemData);
	itsIconData->RemoveElement(index);

	JXMenuData::DeleteItem(index);

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 DeleteAll (virtual)

 ******************************************************************************/

void
JXImageMenuData::DeleteAll()
{
	const JSize itemCount = itsIconData->GetElementCount();
	for (JIndex i=1; i<=itemCount; i++)
		{
		IconData itemData = itsIconData->GetElement(i);
		CleanOutIconItem(&itemData);
		}
	itsIconData->RemoveAll();

	JXMenuData::DeleteAll();

	itsNeedGeomRecalcFlag = kJTrue;
}

/******************************************************************************
 CleanOutIconItem (private)

 ******************************************************************************/

void
JXImageMenuData::CleanOutIconItem
	(
	IconData* itemData
	)
{
	if (itemData->ownsImage)
		{
		jdelete (itemData->image);
		}
	itemData->image = nullptr;
}

/******************************************************************************
 SetImage

 ******************************************************************************/

void
JXImageMenuData::SetImage
	(
	const JIndex	index,
	JXImage*		image,
	const JBoolean	menuOwnsImage
	)
{
	assert( image != nullptr );

	IconData itemData = itsIconData->GetElement(index);

	if (itemData.image == nullptr ||
		(itemData.image->GetBounds()) != image->GetBounds())
		{
		itsNeedGeomRecalcFlag = kJTrue;
		}

	if (itemData.ownsImage)
		{
		jdelete itemData.image;
		}

	itemData.image     = image;
	itemData.ownsImage = menuOwnsImage;
	itsIconData->SetElement(index, itemData);

	Broadcast(ImageChanged(index));
}

/******************************************************************************
 ConfigureTable

	Called by JXImageMenuTable constructor.
	Returns kJTrue if any of the items has a submenu.

 ******************************************************************************/

void
JXImageMenuData::ConfigureTable
	(
	JXImageMenuTable*	table,
	JBoolean*			hasCheckboxes,
	JBoolean*			hasSubmenus
	)
{
	*hasCheckboxes = HasCheckboxes();
	*hasSubmenus   = HasSubmenus();

	if (itsNeedGeomRecalcFlag)
		{
		itsNeedGeomRecalcFlag = kJFalse;
		itsRowHeight          = kMinCellSize;
		itsColWidth           = kMinCellSize;

		const JCoordinate bw = 2*(JXImageMenuTable::kHilightBorderWidth + 1);

		const JSize itemCount = itsIconData->GetElementCount();
		for (JIndex i=1; i<=itemCount; i++)
			{
			const IconData itemData = itsIconData->GetElement(i);
			itsRowHeight = JMax(itsRowHeight, (itemData.image)->GetHeight() + bw);
			itsColWidth  = JMax(itsColWidth, (itemData.image)->GetWidth() + bw);
			}

		if (*hasCheckboxes)
			{
			itsColWidth += JXMenuTable::kCheckboxColWidth;
			}
		if (*hasSubmenus)
			{
			itsColWidth += JXMenuTable::kSubmenuColWidth;
			}
		}

	table->SetAllRowHeights(itsRowHeight);
	table->SetAllColWidths(itsColWidth);
	table->SetDefaultRowHeight(itsRowHeight);
}
