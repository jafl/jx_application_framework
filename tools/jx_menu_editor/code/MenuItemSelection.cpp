/******************************************************************************
 MenuItemSelection.cpp

	text/x-jx-menu-item with DELETE support for DND.

	BASE CLASS = JXSelectionData

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "MenuItemSelection.h"
#include "MenuTable.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kMenuItemXAtomName = "text/x-jx-menu-item";

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuItemSelection::MenuItemSelection
	(
	JXDisplay*					display,
	MenuTable*					table,
	const MenuTable::ItemInfo&	itemInfo
	)
	:
	JXSelectionData(display),
	itsTable(table)
{
	assert( itsTable != nullptr );

	JPoint cell;
	const bool hasSelection =
		itsTable->GetTableSelection().GetSingleSelectedCell(&cell);
	assert( hasSelection );
	itsSrcRowIndex = cell.y;

	std::ostringstream data;
	table->WriteMenuItem(data, itemInfo);
	data << std::ends;
	itsData = data.str();

	ClearWhenGoingAway(itsTable, &itsTable);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MenuItemSelection::~MenuItemSelection()
{
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
MenuItemSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsMenuItemXAtom = AddType(kMenuItemXAtomName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
MenuItemSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	JPoint cell;

	if (requestType == itsMenuItemXAtom)
	{
		*returnType = itsMenuItemXAtom;
		*dataLength = itsData.GetByteCount();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
		{
			memcpy(*data, itsData.GetRawBytes(), *dataLength);
			return true;
		}
	}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTable != nullptr && IsCurrent() &&
			 itsTable->GetTableSelection().GetSingleSelectedCell(&cell) &&
			 JIndex(cell.y) == itsSrcRowIndex)
	{
		itsTable->RemoveSelectedItem();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return true;
	}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return false;
}

/******************************************************************************
 GetMenuItemXAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
MenuItemSelection::GetMenuItemXAtomName()
{
	return kMenuItemXAtomName;
}
