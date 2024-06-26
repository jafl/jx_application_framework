/******************************************************************************
 JXImageMenuData.cpp

	Stores an image for each menu item.

	BASE CLASS = JXMenuData

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXImageMenuData.h"
#include "JXImageMenuTable.h"
#include "JXImage.h"
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

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
	itsIconData    = jnew JArray<IconData>;
	itsColumnCount = columnCount;

	itsNeedGeomRecalcFlag = true;
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
	const bool			menuOwnsImage,
	const JXMenu::ItemType	type,
	const JString&			id
	)
{
	assert( image != nullptr );

	const IconData itemData(image, menuOwnsImage);
	itsIconData->InsertItemAtIndex(index, itemData);

	JXMenuData::InsertItem(index, type, JString::empty, id);

	itsNeedGeomRecalcFlag = true;
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
	IconData itemData = itsIconData->GetItem(index);
	CleanOutIconItem(&itemData);
	itsIconData->RemoveItem(index);

	JXMenuData::DeleteItem(index);

	itsNeedGeomRecalcFlag = true;
}

/******************************************************************************
 DeleteAll (virtual)

 ******************************************************************************/

void
JXImageMenuData::DeleteAll()
{
	for (auto itemData : *itsIconData)
	{
		CleanOutIconItem(&itemData);
	}
	itsIconData->RemoveAll();

	JXMenuData::DeleteAll();

	itsNeedGeomRecalcFlag = true;
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
		jdelete itemData->image;
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
	const bool	menuOwnsImage
	)
{
	assert( image != nullptr );

	IconData itemData = itsIconData->GetItem(index);

	if (itemData.image == nullptr ||
		(itemData.image->GetBounds()) != image->GetBounds())
	{
		itsNeedGeomRecalcFlag = true;
	}

	if (itemData.ownsImage)
	{
		jdelete itemData.image;
	}

	itemData.image     = image;
	itemData.ownsImage = menuOwnsImage;
	itsIconData->SetItem(index, itemData);

	Broadcast(ImageChanged(index));
}

/******************************************************************************
 ConfigureTable

	Called by JXImageMenuTable constructor.
	Returns true if any of the items has a submenu.

 ******************************************************************************/

void
JXImageMenuData::ConfigureTable
	(
	JXImageMenuTable*	table,
	bool*			hasCheckboxes,
	bool*			hasSubmenus
	)
{
	*hasCheckboxes = HasCheckboxes();
	*hasSubmenus   = HasSubmenus();

	if (itsNeedGeomRecalcFlag)
	{
		itsNeedGeomRecalcFlag = false;
		itsRowHeight          = kMinCellSize;
		itsColWidth           = kMinCellSize;

		const JCoordinate bw = 2*(JXImageMenuTable::kHilightBorderWidth + 1);

		for (const auto& itemData : *itsIconData)
		{
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
