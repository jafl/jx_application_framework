/******************************************************************************
 TestTable.cpp

	BASE CLASS = JTable

	Written by John Lindal.

 ******************************************************************************/

#include "TestTable.h"
#include "JTableSelection.h"
#include "JColorManager.h"
#include "jAssert.h"

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

TestTable::TestTable
	(
	const JCoordinate defRowHeight,
	const JCoordinate defColWidth
	)
	:
	JTable(defRowHeight, defColWidth,
		   JColorManager::GetGrayColor(50),
		   JColorManager::GetDefaultSelectionColor())
{
	const JColorID gray50Color = JColorManager::GetGrayColor(50);
	SetRowBorderInfo(1, gray50Color);	// calls TableSetScrollSteps()
	SetColBorderInfo(1, gray50Color);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTable::~TestTable()
{
}

/******************************************************************************
 Row manipulations

 ******************************************************************************/

void
TestTable::TestInsertRows
	(
	const JIndex		index,
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	InsertRows(index, count, rowHeight);
}

void
TestTable::TestPrependRows
	(
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	PrependRows(count, rowHeight);
}

void
TestTable::TestAppendRows
	(
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	AppendRows(count, rowHeight);
}

void
TestTable::TestRemoveRow
	(
	const JIndex index
	)
{
	RemoveRow(index);
}

void
TestTable::TestRemoveNextRows
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	RemoveNextRows(firstIndex, count);
}

void
TestTable::TestRemovePrevRows
	(
	const JIndex	lastIndex,
	const JSize		count
	)
{
	RemovePrevRows(lastIndex, count);
}

void
TestTable::TestRemoveAllRows()
{
	RemoveAllRows();
}

void
TestTable::TestMoveRow
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	MoveRow(origIndex, newIndex);
}

/******************************************************************************
 Column manipulations

 ******************************************************************************/

void
TestTable::TestInsertCols
	(
	const JIndex		index,
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	InsertCols(index, count, colWidth);
}

void
TestTable::TestPrependCols
	(
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	PrependCols(count, colWidth);
}

void
TestTable::TestAppendCols
	(
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	AppendCols(count, colWidth);
}

void
TestTable::TestRemoveCol
	(
	const JIndex index
	)
{
	RemoveCol(index);
}

void
TestTable::TestRemoveNextCols
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	RemoveNextCols(firstIndex, count);
}

void
TestTable::TestRemovePrevCols
	(
	const JIndex	lastIndex,
	const JSize		count
	)
{
	RemovePrevCols(lastIndex, count);
}

void
TestTable::TestRemoveAllCols()
{
	RemoveAllCols();
}

void
TestTable::TestMoveCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	MoveCol(origIndex, newIndex);
}

/******************************************************************************
 TableRefresh (virtual protected)

 ******************************************************************************/

void
TestTable::TableRefresh()
{
}

/******************************************************************************
 TableRefreshRect (virtual protected)

 ******************************************************************************/

void
TestTable::TableRefreshRect
	(
	const JRect& rect
	)
{
}

/******************************************************************************
 TableSetGUIBounds (virtual protected)

 ******************************************************************************/

void
TestTable::TableSetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
}

/******************************************************************************
 TableSetScrollSteps (virtual protected)

 ******************************************************************************/

void
TestTable::TableSetScrollSteps
	(
	const JCoordinate hStep,
	const JCoordinate vStep
	)
{
}

/******************************************************************************
 Adjusting scrolltabs (virtual protected)

 ******************************************************************************/

void
TestTable::TableHeightChanged
	(
	const JCoordinate y,
	const JCoordinate delta
	)
{
}

void
TestTable::TableHeightScaled
	(
	const JFloat scaleFactor
	)
{
}

void
TestTable::TableRowMoved
	(
	const JCoordinate	origY,
	const JSize			height,
	const JCoordinate	newY
	)
{
}

void
TestTable::TableWidthChanged
	(
	const JCoordinate x,
	const JCoordinate delta
	)
{
}

void
TestTable::TableWidthScaled
	(
	const JFloat scaleFactor
	)
{
}

void
TestTable::TableColMoved
	(
	const JCoordinate	origX,
	const JSize			width,
	const JCoordinate	newX
	)
{
}

/******************************************************************************
 TableScrollToCellRect (virtual protected)

 ******************************************************************************/

bool
TestTable::TableScrollToCellRect
	(
	const JRect&	cellRect,
	const bool		centerInDisplay
	)
{
	return false;
}

/******************************************************************************
 TableGetApertureWidth (virtual)

 ******************************************************************************/

JCoordinate
TestTable::TableGetApertureWidth()
	const
{
	return 50;
}

/******************************************************************************
 TableDrawCell (virtual)

 ******************************************************************************/

void
TestTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
}
