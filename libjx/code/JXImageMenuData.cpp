/******************************************************************************
 JXImageMenuData.cpp

	Stores an image for each menu item.

	BASE CLASS = JXMenuData

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageMenuData.h>
#include <JXImageMenuTable.h>
#include <JXImage.h>
#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kMinCellSize = 14;	// 10 + 2+2

// JBroadcaster message types

const JCharacter* JXImageMenuData::kImageChanged  = "ImageChanged::JXImageMenuData";

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
	itsIconData = new JArray<IconData>;
	assert( itsIconData != NULL );

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
	delete itsIconData;
}

/******************************************************************************
 InsertItem

 ******************************************************************************/

void
JXImageMenuData::InsertItem
	(
	const JIndex		index,
	JXImage*			image,
	const JBoolean		menuOwnsImage,
	const JBoolean		isCheckbox,
	const JBoolean		isRadio,
	const JCharacter*	id
	)
{
	assert( image != NULL );

	const IconData itemData(image, menuOwnsImage);
	itsIconData->InsertElementAtIndex(index, itemData);

	JXMenuData::InsertItem(index, isCheckbox, isRadio, NULL, id);

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
		delete (itemData->image);
		}
	itemData->image = NULL;
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
	assert( image != NULL );

	IconData itemData = itsIconData->GetElement(index);

	if (itemData.image == NULL ||
		(itemData.image->GetBounds()) != image->GetBounds())
		{
		itsNeedGeomRecalcFlag = kJTrue;
		}

	if (itemData.ownsImage)
		{
		delete itemData.image;
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

#define JTemplateType JXImageMenuData::IconData
#include <JArray.tmpls>
#undef JTemplateType
