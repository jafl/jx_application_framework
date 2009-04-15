/******************************************************************************
 SimpleTable.cpp

	This is a basic table. All of the data is hard coded, so it can't be
	changed in any way. The purpose of this table is demonstrate how tables
	are created and drawn.

	BASE CLASS = JXTable

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include <JXStdInc.h>
#include "SimpleTable.h"
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <JString.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jAssert.h>

// The default row height and column width.
const JCoordinate kDefRowHeight	= 20;
const JCoordinate kDefColWidth	= 80;

/******************************************************************************
 Constructor

 ******************************************************************************/

SimpleTable::SimpleTable
	(
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
	// We arbitrarily choose to have one column and 10 rows.
	AppendCols(1, kDefColWidth);
	AppendRows(10, kDefRowHeight);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SimpleTable::~SimpleTable()
{
}

/******************************************************************************
 TableDrawCell

	This gets called by JTable every time the cell passed to
	the function needs to be redrawn.

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
SimpleTable::TableDrawCell
	(
	JPainter& 		p, 
	const JPoint& 	cell, 
	const JRect& 	rect
	)
{
	// Convert the row number into a JString.
	JString cellNumber(cell.y);

	// Draw the JString that holds the value. 
	p.String(rect, cellNumber, JPainter::kHAlignLeft, JPainter::kVAlignTop);
}
