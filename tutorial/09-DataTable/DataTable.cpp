/******************************************************************************
 DataTable.cpp

	The table is extended to support a data array. The table listens for
	changed to the data, and adjusts appropriately when the data changes.

	BASE CLASS = JXTable

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "DataTable.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/jAssert.h>

// The default row height and column width.
const JCoordinate kDefRowHeight	= 20;
const JCoordinate kDefColWidth	= 80;

/******************************************************************************
 Constructor

 ******************************************************************************/

DataTable::DataTable
	(
	JArray<JIndex>* 	data,
	JXScrollbarSet* 	scrollbarSet, 
	JXContainer* 		enclosure,
	const HSizingOption hSizing, 
	const VSizingOption vSizing,
	const JCoordinate 	x, 
	const JCoordinate 	y,
	const JCoordinate 	w, 
	const JCoordinate 	h
	)
	:
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	// This will be our data, but we don't own it so we can't delete it.
	itsData = data;

	// This data needs only one column
	AppendCols(1, kDefColWidth);

	// We need to add a row for each element in the data array
	AppendRows(itsData->GetItemCount(), kDefRowHeight);

	// The table is now in sync with the data array, but in 
	// order to hear about changes in the data, we have to listen
	// for messages from the data.
	ListenTo(itsData, std::function([this](const JListT::ItemsInserted& msg)
	{
		// For each element inserted, we insert a row
		InsertRows(msg.GetFirstIndex(), msg.GetCount(), kDefRowHeight);
	}));

	ListenTo(itsData, std::function([this](const JListT::ItemsRemoved& msg)
	{
		// Remove the corresponding table rows. 
		RemoveNextRows(msg.GetFirstIndex(), msg.GetCount());
	}));

	ListenTo(itsData, std::function([this](const JListT::ItemsChanged& msg)
	{
		// The element changed, so redraw it.
		// (This would not be necessary if we were using a
		//  class derived from JTableData.)
		for (JIndex i=msg.GetFirstIndex(); i<=msg.GetLastIndex(); i++)
		{
			TableRefreshCell(i, 1);
		}
	}));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DataTable::~DataTable()
{
	// We don't own itsData, so we don't delete it.
}

/******************************************************************************
 TableDrawCell

	This gets called by JTable every time the table needs to
	be redrawn.

	p is the Painter that you use to draw to the screen.  The main reason
	for Painter is to hide the system dependent details of drawing,
	and to provide a uniform interface for drawing to the screen, to an
	offscreen image, and to a printer.

	rect is the boundary of the cell.  The clipping rectangle has been set
	set to this so anything that you draw outside this rectangle will not
	be displayed.

	cell gives the row and column of the cell that needs to be redrawn.
	
	cell.x = column
	cell.y = row

 ******************************************************************************/

void
DataTable::TableDrawCell
	(
	JPainter& 		p, 
	const JPoint& 	cell, 
	const JRect& 	rect
	)
{
	// Convert the array's current element into a JString.
	JString cellNumber((JUInt64) itsData->GetItem(cell.y));

	// Draw the JString that holds the value. 
	p.String(rect, cellNumber, JPainter::HAlign::kLeft, JPainter::VAlign::kTop);
}
