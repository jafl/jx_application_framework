/******************************************************************************
 JTable.cpp

	Abstract class to define a display organized into rows and columns.
	JPoint(col,row) indicates a particular cell.

	The borders between rows are not counted in the width of the rows,
	and likewise for columns.

	Derived classes must implement the following routines:

		TableDrawCell
			Draw the contents of the given cell.  All drawing is automatically
			clipped to the boundaries of the cell.  This can usually only be
			overridden by leaf classes since it requires understanding of the
			actual data.  The origin of the painter is irrelevant because
			the cellRect provides all the necessary information.

		TableSetGUIBounds
			The total size of the table has changed to the given width and height.

		TableSetScrollSteps
			Set the step sizes and page contexts used when scrolling the table.

		TableHeightChanged
		TableHeightScaled
		TableRowMoved
		TableWidthChanged
		TableWidthScaled
		TableColMoved
			Adjust the scrolltabs.

		TableScrollToCell
			Scroll the table to display the specified cell rectangle.
			Return kJFalse if no scrolling was necessary.

		TableGetApertureWidth
			Return the width of the visible portion of the table.

		TableRefresh
			Refresh the entire table.

		TableRefreshRect
			Refresh the specified rectangle (in pixels).

	Derived classes can override the following routines:

		TablePrepareToDrawRow
			DrawCell is about to be called for each cell in a row.  If some
			common initialization needs to be done, do it here.

		TablePrepareToDrawCol
			DrawCell is about to be called for each cell in a column.  If some
			common initialization needs to be done, do it here.

	To use our cell selection functions, a derived class should consider
	overriding:

		IsSelectable
			Returns kJTrue if the specified cell is selectable.

	To use our editing mechanism, a derived class must override the
	following routines:

		IsEditable
			Return kJTrue if the specified cell can be edited.

		CreateInputField
			Create or activate an input field to edit the specified cell.
			Returns kJTrue if successful.

		ExtractInputData
			Check the data in the active input field, and save it if
			it is valid.  Return kJTrue if it is valid.

		DeleteInputField
			Delete or deactivate the active input field.  Called when
			editing is cancelled or when ExtractInputData returns kJTrue.

		PlaceInputField
		MoveInputField
		SetInputFieldSize
		ResizeInputField
			Adjust the position or size of the input field.

	We could have written this so that the input field moved itself based
	on JTable messages, but this would require every input field object
	to implement the same Receive() code, which is a waste.  It seems much
	better to do it once in JTable code.

	To draw page headers and footers while printing, override the
	following routines:

		GetPrintHeaderHeight
			Return the height required for the page header.

		DrawPrintHeader
			Draw the page header.  JTable will lock the header afterwards.

		GetPrintFooterHeight
			Return the height required for the page footer.

		DrawPrintFooter
			Draw the page footer.  JTable will lock the footer afterwards.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTable.h>
#include <JTableSelection.h>
#include <JPagePrinter.h>
#include <JEPSPrinter.h>
#include <JOrderedSetUtil.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Derived classes must call TableSetScrollSteps because it is
	pure virtual for us.

 ******************************************************************************/

JTable::JTable
	(
	const JCoordinate	defRowHeight,
	const JCoordinate	defColWidth,
	const JColorIndex	borderColor,
	const JColorIndex	selectionColor
	)
	:
	itsRowBorderInfo(0, borderColor),
	itsColBorderInfo(0, borderColor),
	itsSelectionColor(selectionColor)
{
	itsDrawOrder    = kDrawByCol;
	itsIs1DListFlag = kJFalse;

	itsWidth = itsHeight = 0;

	itsDefRowHeight = defRowHeight;
	itsDefColWidth  = defColWidth;

	itsRowHeights = new JRunArray<JCoordinate>;
	assert( itsRowHeights != NULL );

	itsColWidths = new JRunArray<JCoordinate>;
	assert( itsColWidths != NULL );

	itsTableData = NULL;

	itsAuxDataList = new JPtrArray<JBroadcaster>(JPtrArrayT::kForgetAll);
	assert( itsAuxDataList != NULL );

	itsIsEditingFlag = kJFalse;

	itsSelDragType             = kInvalidDrag;
	itsAllowSelectMultipleFlag = kJFalse;
	itsAllowSelectDiscontFlag  = kJFalse;

	itsRowHdrTable = NULL;
	itsColHdrTable = NULL;

	itsTableSelection = new JTableSelection(this);		// calls RegisterAuxData()
	assert( itsTableSelection != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTable::~JTable()
{
	delete itsTableSelection;	// calls RemoveAuxData()

	delete itsRowHeights;
	delete itsColWidths;
	delete itsAuxDataList;
}

/******************************************************************************
 SetTableData

 ******************************************************************************/

void
JTable::SetTableData
	(
	const JTableData* data
	)
{
	CancelEditing();

	if (data != itsTableData)
		{
		if (itsTableData != NULL)
			{
			StopListening(itsTableData);
			}

		itsTableData = data;

		if (itsTableData != NULL)
			{
			ListenTo(itsTableData);
			}
		}

	AdjustToTableData();
}

/******************************************************************************
 AdjustToTableData (protected)

 ******************************************************************************/

void
JTable::AdjustToTableData()
{
	Broadcast(PrepareForTableDataChange());

	if (itsTableData != NULL)
		{
		const JSize dataRowCount = itsTableData->GetRowCount();
		if (GetRowCount() < dataRowCount)
			{
			AppendRows(dataRowCount - GetRowCount(), itsDefRowHeight);
			}
		else if (GetRowCount() > dataRowCount)
			{
			RemoveNextRows(dataRowCount+1, GetRowCount() - dataRowCount);
			}

		const JSize dataColCount = itsTableData->GetColCount();
		if (GetColCount() < dataColCount)
			{
			AppendCols(dataColCount - GetColCount(), itsDefColWidth);
			}
		else if (GetColCount() > dataColCount)
			{
			RemoveNextCols(dataColCount+1, GetColCount() - dataColCount);
			}
		}
	else
		{
		RemoveAllRows();
		RemoveAllCols();
		}

	Broadcast(TableDataChanged());
}

/******************************************************************************
 RegisterAuxData

	We have to take JTableData instead of JAuxTableData because we don't want
	to have to use templates.

 ******************************************************************************/

void
JTable::RegisterAuxData
	(
	JTableData* auxData
	)
{
	if (!itsAuxDataList->Includes(auxData))
		{
		itsAuxDataList->Append(auxData);
		ListenTo(auxData);
		}
}

/******************************************************************************
 RemoveAuxData

	We have to take JTableData instead of JAuxTableData because we don't want
	to have to use templates.

 ******************************************************************************/

void
JTable::RemoveAuxData
	(
	JTableData* auxData
	)
{
	itsAuxDataList->Remove(auxData);
	StopListening(auxData);
}

/******************************************************************************
 GetCellSize (static private)

 ******************************************************************************/

static JInteger sBorderWidth = 0;

JInteger
JTable::GetCellSize
	(
	const JCoordinate& value
	)
{
	return value + sBorderWidth;
}

/******************************************************************************
 TableRefreshCellRect

 ******************************************************************************/

void
JTable::TableRefreshCellRect
	(
	const JRect& cellRect
	)
{
	const JPoint firstCell = cellRect.topLeft();
	const JPoint lastCell  = cellRect.bottomRight() - JPoint(1,1);
	if (firstCell.x <= lastCell.x && firstCell.y <= lastCell.y)
		{
		JRect pixelRect = JCovering(GetCellRect(firstCell), GetCellRect(lastCell));
		TableRefreshRect(pixelRect);
		}
}

/******************************************************************************
 TableDraw

	When calling TableDrawCell(), the painter's origin is at the top left
	of the table bounds.  This forces everybody to write clean code because
	they have to use cellRect in all their formulas.

	If drawLowerRightBorder, we draw the border to close the cells along
	the lower and right edges of the table.

 ******************************************************************************/

void
JTable::TableDraw
	(
	JPainter&		p,
	const JRect&	r,
	const JBoolean	drawLowerRightBorder
	)
{
	// determine which cells to draw

	JIndex firstRow, lastRow, firstCol, lastCol;
	if (!GetVisibleRange(r.top, r.bottom, *itsRowHeights, itsRowBorderInfo.width,
						 &firstRow, &lastRow) ||
		!GetVisibleRange(r.left, r.right, *itsColWidths, itsColBorderInfo.width,
						 &firstCol, &lastCol))
		{
		return;
		}

	// draw the cell borders

	TableDrawRowBorders(p, r, firstRow, lastRow, drawLowerRightBorder);
	TableDrawColBorders(p, r, firstCol, lastCol, drawLowerRightBorder);

	// draw the cells

	if (itsDrawOrder == kDrawByRow)
		{
		JRunArrayIterator<JCoordinate> iter(itsColWidths);

		for (JIndex i=firstRow; i<=lastRow; i++)
			{
			TablePrepareToDrawRow(i, firstCol, lastCol);
			iter.MoveTo(kJIteratorStartAfter, firstCol);

			JRect cellRect = GetCellRect(JPoint(firstCol, i));
			for (JIndex j=firstCol; j<=lastCol; j++)
				{
				p.ResetAllButClipping();
				p.SetClipRect(cellRect);
				TableDrawCell(p, JPoint(j,i), cellRect);

				if (j < lastCol)
					{
					JCoordinate colWidth;
					const JBoolean ok = iter.Next(&colWidth);
					assert( ok );
					cellRect.left  = cellRect.right + itsColBorderInfo.width;
					cellRect.right = cellRect.left  + colWidth;
					}
				}
			}
		}
	else
		{
		assert( itsDrawOrder == kDrawByCol );

		JRunArrayIterator<JCoordinate> iter(itsRowHeights);

		for (JIndex j=firstCol; j<=lastCol; j++)
			{
			TablePrepareToDrawCol(j, firstRow, lastRow);
			iter.MoveTo(kJIteratorStartAfter, firstRow);

			JRect cellRect = GetCellRect(JPoint(j, firstRow));
			for (JIndex i=firstRow; i<=lastRow; i++)
				{
				p.ResetAllButClipping();
				p.SetClipRect(cellRect);
				TableDrawCell(p, JPoint(j,i), cellRect);

				if (i < lastRow)
					{
					JCoordinate rowHeight;
					const JBoolean ok = iter.Next(&rowHeight);
					assert( ok );
					cellRect.top    = cellRect.bottom + itsRowBorderInfo.width;
					cellRect.bottom = cellRect.top    + rowHeight;
					}
				}
			}
		}

	p.ResetClipRect();
}

/******************************************************************************
 Draw borders (private)

	We need the rectangle because the table might be so large that itsWidth
	or itsHeight is too large for the system (which normally uses 16 bits).
	JPainter subtracts off the origin, so the given rectangle will always
	translate to values that fit in 16 bits.

 ******************************************************************************/

void
JTable::TableDrawRowBorders
	(
	JPainter&		p,
	const JRect&	r,
	const JIndex	firstRow,
	const JIndex	lastRow,
	const JBoolean	drawBottomBorder
	)
{
	if (itsRowBorderInfo.width <= 0)
		{
		return;
		}

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(itsRowBorderInfo.width);
	p.SetPenColor(itsRowBorderInfo.color);

	JRunArrayIterator<JCoordinate> iter(itsRowHeights, kJIteratorStartBefore, firstRow);

	const JSize rowCount = GetRowCount();
	sBorderWidth         = itsRowBorderInfo.width;
	JCoordinate y        = -itsRowBorderInfo.width + (itsRowBorderInfo.width / 2) +	// thick line is centered on path
						   (firstRow == 1 ? 0 : itsRowHeights->SumElements(1, firstRow-1, GetCellSize));
	for (JIndex i=firstRow;
		 i <= lastRow &&
		 (( drawBottomBorder && i<=rowCount) ||
		  (!drawBottomBorder && i< rowCount));
		 i++)
		{
		JCoordinate rowHeight;
		const JBoolean ok = iter.Next(&rowHeight);
		assert( ok );

		y += rowHeight + itsRowBorderInfo.width;

		p.Line(r.left,y, r.right-1+itsColBorderInfo.width,y);
		}

	p.SetLineWidth(origWidth);
	p.SetPenColor(origColor);
}

void
JTable::TableDrawColBorders
	(
	JPainter&		p,
	const JRect&	r,
	const JIndex	firstCol,
	const JIndex	lastCol,
	const JBoolean	drawRightBorder
	)
{
	if (itsColBorderInfo.width <= 0)
		{
		return;
		}

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(itsColBorderInfo.width);
	p.SetPenColor(itsColBorderInfo.color);

	JRunArrayIterator<JCoordinate> iter(itsColWidths, kJIteratorStartBefore, firstCol);

	const JSize colCount = GetColCount();
	sBorderWidth         = itsColBorderInfo.width;
	JCoordinate x        = -itsColBorderInfo.width + (itsColBorderInfo.width / 2) +	// thick line is centered on path
						   (firstCol == 1 ? 0 : itsColWidths->SumElements(1, firstCol-1, GetCellSize));
	for (JIndex i=firstCol;
		 i <= lastCol &&
		 (( drawRightBorder && i<=colCount) ||
		  (!drawRightBorder && i< colCount));
		 i++)
		{
		JCoordinate colWidth;
		const JBoolean ok = iter.Next(&colWidth);
		assert( ok );

		x += colWidth + itsColBorderInfo.width;

		p.Line(x,r.top, x,r.bottom-1+itsRowBorderInfo.width);
		}

	p.SetLineWidth(origWidth);
	p.SetPenColor(origColor);
}

/******************************************************************************
 GetVisibleRange (private)

 ******************************************************************************/

JBoolean
JTable::GetVisibleRange
	(
	const JCoordinate		min,
	const JCoordinate		max,
	JRunArray<JCoordinate>&	lengths,
	const JCoordinate		borderWidth,

	JIndex* firstIndex,
	JIndex* lastIndex
	)
	const
{
	assert( 0 <= min && min <= max );

	if (lengths.IsEmpty() || min == max)
		{
		return kJFalse;
		}

	JInteger v;
	sBorderWidth = borderWidth;
	lengths.FindPositiveSum(min,       1,           firstIndex, &v, GetCellSize);
	lengths.FindPositiveSum(max-1 - v, *firstIndex, lastIndex,  &v, GetCellSize);
	return kJTrue;
/*
	JRunArrayIterator<JCoordinate> iter(&lengths);

	const JSize count = lengths.GetElementCount();

	*firstIndex = 0;
	JCoordinate totalLength = 0;
	while (*firstIndex < count && totalLength <= min)
		{
		(*firstIndex)++;
		JCoordinate length;
		const JBoolean ok = iter.Next(&length);
		assert( ok );
		totalLength += length + borderWidth;
		}

	*lastIndex = *firstIndex;
	while (*lastIndex < count && totalLength < max)
		{
		(*lastIndex)++;
		JCoordinate length;
		const JBoolean ok = iter.Next(&length);
		assert( ok );
		totalLength += length + borderWidth;
		}

	return kJTrue;
*/
}

/******************************************************************************
 TablePrepareToDrawRow (virtual protected)

 ******************************************************************************/

void
JTable::TablePrepareToDrawRow
	(
	const JIndex rowIndex,
	const JIndex firstCol,
	const JIndex lastCol
	)
{
}

/******************************************************************************
 TablePrepareToDrawCol (virtual protected)

 ******************************************************************************/

void
JTable::TablePrepareToDrawCol
	(
	const JIndex colIndex,
	const JIndex firstRow,
	const JIndex lastRow
	)
{
}

/******************************************************************************
 HilightIfSelected (protected)

	Make this the first call in TableDrawCell() to automatically hilight
	all selected cells in a table.

 ******************************************************************************/

void
JTable::HilightIfSelected
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
	const
{
	if (itsTableSelection->IsSelected(cell))
		{
		const JColorIndex origColor = p.GetPenColor();
		const JBoolean origFill     = p.IsFilling();

		p.SetPenColor(itsSelectionColor);
		p.SetFilling(kJTrue);
		p.Rect(rect);

		p.SetPenColor(origColor);
		p.SetFilling(origFill);
		}
}

/******************************************************************************
 Paginate (protected)

	Returns breakpoints for cutting table into pages.  The first breakpoint
	is always zero, and the last breakpoint is the width or height of
	the table.  Thus, it is easy to calculate the width of what is printed
	on each page from (breakpt->GetElement(i+1) - breakpt->GetElement(i) + 1).

 ******************************************************************************/

JBoolean
JTable::Paginate
	(
	const JCoordinate		origPageWidth,
	const JCoordinate		origPageHeight,
	const JBoolean			userPrintRowHeader,
	JArray<JCoordinate>*	rowBreakpts,
	JBoolean*				printRowHeader,
	const JBoolean			userPrintColHeader,
	JArray<JCoordinate>*	colBreakpts,
	JBoolean*				printColHeader
	)
	const
{
	JCoordinate pageWidth  = origPageWidth;
	JCoordinate pageHeight = origPageHeight;

	*printRowHeader = kJFalse;
	if (userPrintRowHeader &&
		itsRowHdrTable != NULL && itsRowHdrTable->itsWidth < pageWidth)
		{
		pageWidth      -= itsRowHdrTable->itsWidth;
		*printRowHeader = kJTrue;
		}

	*printColHeader = kJFalse;
	if (userPrintColHeader &&
		itsColHdrTable != NULL && itsColHdrTable->itsHeight < pageHeight)
		{
		pageHeight     -= itsColHdrTable->itsHeight;
		*printColHeader = kJTrue;
		}

	return JConvertToBoolean(
		Paginate(pageHeight, *itsRowHeights, itsRowBorderInfo.width, rowBreakpts) &&
		Paginate(pageWidth,  *itsColWidths,  itsColBorderInfo.width, colBreakpts) );
}

// private

JBoolean
JTable::Paginate
	(
	const JCoordinate		stripLength,
	JRunArray<JCoordinate>&	lengths,
	const JCoordinate		borderWidth,
	JArray<JCoordinate>*	breakpts
	)
	const
{
	assert( stripLength > 0 );

	if (lengths.IsEmpty())
		{
		return kJFalse;
		}

	breakpts->RemoveAll();
	breakpts->AppendElement(0);

	JRunArrayIterator<JCoordinate> iter(&lengths);

	const JSize count = lengths.GetElementCount();

	JIndex prev = 1, i = 0;
	JCoordinate totalLength = 0;
	do
		{
		// This shift to the left allows all but one pixel of
		// the rightmost border to fall off the edge of the page.

		if (borderWidth > 1)
			{
			totalLength -= borderWidth - 1;
			}

		// find the number of strips that will fit on this page

		while (i < count && totalLength <= stripLength)
			{
			i++;
			JCoordinate length;
			const JBoolean ok = iter.Next(&length);
			assert( ok );
			totalLength += length + borderWidth;
			}

		JCoordinate pageLen = totalLength;
		if (totalLength > stripLength && i > prev)
			{
			// The last strip didn't fit on the page,
			// so leave it for the next page.

			JCoordinate length;
			const JBoolean ok = iter.Prev(&length);
			assert( ok );
			pageLen -= length + borderWidth;
			i--;
			totalLength = 0;
			}
		else if (totalLength > stripLength)
			{
			// The strip won't fit on any page.  Put
			// as much as possible on this page and leave
			// the rest for the next page.

			pageLen      = stripLength;
			totalLength -= stripLength;
			}
		else
			{
			// Everything fits on the page, so there is no residual.

			totalLength = 0;
			}

		JCoordinate newBreakpt = breakpts->GetLastElement() + pageLen;
		if (borderWidth > 1)
			{
			newBreakpt += borderWidth - 1;		// compensate for initial shift
			}

		breakpts->AppendElement(newBreakpt);
		prev = i;
		}
		while (i < count || totalLength > 0);

	// last element isn't followed by border

	if (borderWidth > 1)
		{
		const JCoordinate lastBreakpt = breakpts->GetLastElement();
		breakpts->SetElement(breakpts->GetElementCount(), lastBreakpt - borderWidth+1);
		}

	return kJTrue;
}

/******************************************************************************
 Print

	PostScript

 ******************************************************************************/

void
JTable::Print
	(
	JPagePrinter&	p,
	const JBoolean	userPrintRowHeader,
	const JBoolean	userPrintColHeader
	)
{
	assert( !itsIsEditingFlag );

	if (!p.OpenDocument())
		{
		return;
		}

	const JCoordinate headerHeight = GetPrintHeaderHeight(p);
	const JCoordinate footerHeight = GetPrintFooterHeight(p);

	JArray<JCoordinate> rowBreakpts, colBreakpts;
	JBoolean printRowHeader, printColHeader;
	const JBoolean ok =
		Paginate(p.GetPageWidth(), p.GetPageHeight()-headerHeight-footerHeight,
				 userPrintRowHeader, &rowBreakpts, &printRowHeader,
				 userPrintColHeader, &colBreakpts, &printColHeader);
	assert( ok );

	const JSize rowCount = rowBreakpts.GetElementCount() - 1;
	const JSize colCount = colBreakpts.GetElementCount() - 1;

	JBoolean cancelled = kJFalse;
	for (JIndex j=1; j<=colCount; j++)
		{
		for (JIndex i=1; i<=rowCount; i++)
			{
			if (!p.NewPage())
				{
				cancelled = kJTrue;
				break;
				}

			// draw header and footer

			if (headerHeight > 0)
				{
				DrawPrintHeader(p, headerHeight);
				p.LockHeader(headerHeight);
				}
			if (footerHeight > 0)
				{
				DrawPrintFooter(p, footerHeight);
				p.LockFooter(footerHeight);
				}

			// calculate visible area of table on this page

			JPoint tableTopLeft(colBreakpts.GetElement(j), rowBreakpts.GetElement(i));
			JPoint tableBotRight(colBreakpts.GetElement(j+1), rowBreakpts.GetElement(i+1));

			if (i < rowCount)
				{
				tableBotRight.y -= itsRowBorderInfo.width;
				}
			if (j < colCount)
				{
				tableBotRight.x -= itsColBorderInfo.width;
				}

			// calculate shift from row and column headers

			JPoint hdrOffset(0,0);
			if (printRowHeader)
				{
				hdrOffset.x = itsRowHdrTable->itsWidth;
				}
			if (printColHeader)
				{
				hdrOffset.y = itsColHdrTable->itsHeight;
				}

			// draw the visible parts of the row and column headers

			if (printRowHeader)
				{
				const JPoint hdrTopLeft(0, tableTopLeft.y);
				const JPoint hdrBotRight(itsRowHdrTable->itsWidth, tableBotRight.y);
				const JPoint offset(0, hdrOffset.y);
				itsRowHdrTable->PrintPage(p, hdrTopLeft, hdrBotRight, offset, kJFalse);
				}

			if (printColHeader)
				{
				const JPoint hdrTopLeft(tableTopLeft.x, 0);
				const JPoint hdrBotRight(tableBotRight.x, itsColHdrTable->itsHeight);
				const JPoint offset(hdrOffset.x, 0);
				itsColHdrTable->PrintPage(p, hdrTopLeft, hdrBotRight, offset, kJFalse);
				}

			// draw the visible part of the table last so it overwrites slop
			// from row and column headers

			PrintPage(p, tableTopLeft, tableBotRight, hdrOffset, kJTrue);
			}

		if (cancelled)
			{
			break;
			}
		}

	if (!cancelled)
		{
		p.CloseDocument();
		}
}

/******************************************************************************
 PrintPage (private)

 ******************************************************************************/

void
JTable::PrintPage
	(
	JPainter&		p,
	const JPoint&	topLeft,
	const JPoint&	botRight,
	const JPoint&	offset,
	const JBoolean	drawFrame
	)
{
	// draw the visible part of the table

	JRect clipRect(JPoint(0,0), botRight - topLeft);
	clipRect.Shift(offset);
	p.SetClipRect(clipRect);

	const JPoint oShift = offset-topLeft;
	p.ShiftOrigin(oShift);
	TableDraw(p, JRect(topLeft,botRight));

	p.Reset();
	p.ShiftOrigin(-oShift);

	// frame it if it has visible border lines

	if (drawFrame && (itsRowBorderInfo.width > 0 || itsColBorderInfo.width > 0))
		{
		p.Rect(clipRect);
		}
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.

 ******************************************************************************/

JCoordinate
JTable::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

JCoordinate
JTable::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

void
JTable::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
}

void
JTable::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
}

/******************************************************************************
 Print

	EPS

 ******************************************************************************/

void
JTable::Print
	(
	JEPSPrinter&	p,
	const JBoolean	userPrintRowHeader,
	const JBoolean	userPrintColHeader
	)
{
	assert( !itsIsEditingFlag );

	// calculate required area

	JRect r(0, 0, itsHeight, itsWidth);

	JBoolean printRowHeader = kJFalse;
	if (userPrintRowHeader && itsRowHdrTable != NULL)
		{
		r.right       += itsRowHdrTable->itsWidth;
		printRowHeader = kJTrue;
		}

	JBoolean printColHeader = kJFalse;
	if (userPrintColHeader && itsColHdrTable != NULL)
		{
		r.bottom      += itsColHdrTable->itsHeight;
		printColHeader = kJTrue;
		}

	// preview

	if (p.WantsPreview())
		{
		JPainter& p1 = p.GetPreviewPainter(r);
		PrintEPS1(p1, JPoint(0,0), printRowHeader, printColHeader);
		}

	// print

	if (p.OpenDocument(r))
		{
		PrintEPS1(p, JPoint(0,0), printRowHeader, printColHeader);
		p.CloseDocument();
		}
}

/******************************************************************************
 PrintEPS1 (private)

 ******************************************************************************/

void
JTable::PrintEPS1
	(
	JPainter&		p,
	const JPoint&	topLeft,
	const JBoolean	printRowHeader,
	const JBoolean	printColHeader
	)
{
	// calculate shift from row and column headers

	JPoint hdrOffset = topLeft;
	if (printRowHeader)
		{
		hdrOffset.x += itsRowHdrTable->itsWidth;
		}
	if (printColHeader)
		{
		hdrOffset.y += itsColHdrTable->itsHeight;
		}

	// draw the visible parts of the row and column headers

	if (printRowHeader)
		{
		const JPoint hdrTopLeft(0,0);
		const JPoint hdrBotRight(itsRowHdrTable->itsWidth, itsHeight);
		const JPoint offset(topLeft.x, hdrOffset.y);
		itsRowHdrTable->PrintPage(p, hdrTopLeft, hdrBotRight, offset, kJFalse);
		}

	if (printColHeader)
		{
		const JPoint hdrTopLeft(0,0);
		const JPoint hdrBotRight(itsWidth, itsColHdrTable->itsHeight);
		const JPoint offset(hdrOffset.x, topLeft.y);
		itsColHdrTable->PrintPage(p, hdrTopLeft, hdrBotRight, offset, kJFalse);
		}

	// draw the table last so it overwrites slop from row and column headers

	const JPoint tableTopLeft(0,0);
	const JPoint tableBotRight(itsWidth, itsHeight);
	PrintPage(p, tableTopLeft, tableBotRight, hdrOffset, kJTrue);
}

/******************************************************************************
 DrawForPrint

	Anywhere on the page

	Not called Print() because it would conflict with other prototypes.

 ******************************************************************************/

void
JTable::DrawForPrint
	(
	JPainter&		p,
	const JPoint&	topLeft,
	const JBoolean	userPrintRowHeader,
	const JBoolean	userPrintColHeader
	)
{
	assert( !itsIsEditingFlag );

	const JBoolean printRowHeader =
		JI2B(userPrintRowHeader && itsRowHdrTable != NULL);
	const JBoolean printColHeader =
		JI2B(userPrintColHeader && itsColHdrTable != NULL);

	PrintEPS1(p, topLeft, printRowHeader, printColHeader);
}

/******************************************************************************
 InsertRows

 ******************************************************************************/

void
JTable::InsertRows
	(
	const JIndex		index,
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	if (count == 0)
		{
		return;
		}

	const JSize rowCount   = GetRowCount();
	const JIndex trueIndex = JMin(index, rowCount+1);

	// update aux data (e.g. selection) if we don't have table data

	JTableData::RowsInserted auxMessage(trueIndex, count);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	JCoordinate h = rowHeight;
	if (rowHeight <= 0 && (trueIndex == 1 || rowCount == 0))
		{
		h = itsDefRowHeight;
		}
	else if (rowHeight <= 0)
		{
		h = itsRowHeights->GetElement(trueIndex-1);
		}

	itsRowHeights->InsertElementsAtIndex(trueIndex, h, count);

	RowsInserted msg(trueIndex, count, h);

	h = count * h + (count-1) * itsRowBorderInfo.width;
	if (rowCount > 0)
		{
		h += itsRowBorderInfo.width;
		}
	TableHeightChanged(GetRowTop(trueIndex), h);
	TableAdjustBounds(0, h);
	Broadcast(msg);
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// move the input field

	if (itsIsEditingFlag)
		{
		JPoint newEditCell = itsEditCell;
		auxMessage.AdjustCell(&newEditCell);
		if (newEditCell != itsEditCell)
			{
			itsEditCell = newEditCell;
			MoveInputField(0, h);
			}
		}
}

/******************************************************************************
 SetRowHeight

 ******************************************************************************/

void
JTable::SetRowHeight
	(
	const JIndex		index,
	const JCoordinate	rowHeight
	)
{
	assert( rowHeight > 0 );

	const JCoordinate oldHeight = itsRowHeights->GetElement(index);
	if (rowHeight != oldHeight)
		{
		itsRowHeights->SetElement(index, rowHeight);

		const JCoordinate deltaH = rowHeight - oldHeight;
		TableHeightChanged(GetRowTop(index) +
						   (deltaH < 0 ? rowHeight : oldHeight), deltaH);

		TableAdjustBounds(0, deltaH);
		Broadcast(RowHeightChanged(index, oldHeight, rowHeight));

		// adjust the input field

		if (itsIsEditingFlag && ((JIndex) itsEditCell.y) == index)
			{
			ResizeInputField(0, rowHeight - oldHeight);
			}
		else if (itsIsEditingFlag && ((JIndex) itsEditCell.y) > index)
			{
			MoveInputField(0, rowHeight - oldHeight);
			}
		}
}

/******************************************************************************
 SetAllRowHeights

 ******************************************************************************/

void
JTable::SetAllRowHeights
	(
	const JCoordinate rowHeight
	)
{
	assert( rowHeight > 0 );

	const JSize rowCount = itsRowHeights->GetElementCount();
	if (rowCount > 0)
		{
		itsRowHeights->RemoveAll();
		itsRowHeights->AppendElements(rowHeight, rowCount);

		const JCoordinate newHeight =
			rowCount * rowHeight + (rowCount-1) * itsRowBorderInfo.width;
		TableHeightScaled(newHeight / (JFloat) itsHeight);
		TableSetBounds(itsWidth, newHeight);
		Broadcast(AllRowHeightsChanged(rowHeight));

		// adjust the input field

		if (itsIsEditingFlag)
			{
			const JRect& editRect = GetCellRect(itsEditCell);
			PlaceInputField(editRect.left, editRect.top);
			SetInputFieldSize(editRect.width(), editRect.height());
			}
		}
}

/******************************************************************************
 RemoveNextRows

 ******************************************************************************/

void
JTable::RemoveNextRows
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	if (firstIndex == 1 && count == GetRowCount())
		{
		RemoveAllRows();
		return;
		}

	// update aux data (e.g. selection) if we don't have table data

	JTableData::RowsRemoved auxMessage(firstIndex, count);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	sBorderWidth                    = itsRowBorderInfo.width;
	const JCoordinate removedY      = GetRowTop(firstIndex);
	const JCoordinate removedHeight = itsRowHeights->SumElements(firstIndex, firstIndex+count-1, GetCellSize);

	itsRowHeights->RemoveNextElements(firstIndex, count);
	TableHeightChanged(removedY, -removedHeight);
	TableAdjustBounds(0, -removedHeight);
	Broadcast(RowsRemoved(firstIndex, count));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// adjust the input field

	if (itsIsEditingFlag)
		{
		JPoint newEditCell = itsEditCell;
		if (!auxMessage.AdjustCell(&newEditCell))
			{
			CancelEditing();
			}
		else if (newEditCell != itsEditCell)
			{
			itsEditCell = newEditCell;
			MoveInputField(0, -removedHeight);
			}
		}
}

/******************************************************************************
 RemoveAllRows

 ******************************************************************************/

void
JTable::RemoveAllRows()
{
	// update aux data (e.g. selection) if we don't have table data

	JTableData::RowsRemoved auxMessage(1, GetRowCount());
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	CancelEditing();

	itsRowHeights->RemoveAll();
	itsHeight = 0;
	TableSetBounds(itsWidth, 0);
	Broadcast(RowsRemoved(1, auxMessage.GetCount()));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}
}

/******************************************************************************
 MoveRow

 ******************************************************************************/

void
JTable::MoveRow
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	// update aux data (e.g. selection) if we don't have table data

	JTableData::RowMoved auxMessage(origIndex, newIndex);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	TableRowMoved(GetRowTop(origIndex),
				  itsRowHeights->GetElement(origIndex) + itsRowBorderInfo.width,
				  GetRowTop(newIndex));

	itsRowHeights->MoveElementToIndex(origIndex, newIndex);
	JRect cellRect(JPoint(1, origIndex), JPoint(GetColCount(), newIndex));
	TableRefreshCellRect(cellRect);
	Broadcast(RowMoved(origIndex, newIndex));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// adjust the input field

	if (itsIsEditingFlag && ((JIndex) itsEditCell.y) == origIndex)
		{
		itsEditCell.y = newIndex;
		const JRect& editRect = GetCellRect(itsEditCell);
		PlaceInputField(editRect.left, editRect.top);
		}
	else if (itsIsEditingFlag && origIndex < ((JIndex) itsEditCell.y) &&
			 ((JIndex) itsEditCell.y) <= newIndex)
		{
		(itsEditCell.y)--;
		MoveInputField(0, -GetRowHeight(newIndex) - itsRowBorderInfo.width);
		}
	else if (itsIsEditingFlag && newIndex <= ((JIndex) itsEditCell.y) &&
			 ((JIndex) itsEditCell.y) < origIndex)
		{
		(itsEditCell.y)++;
		MoveInputField(0, GetRowHeight(newIndex) + itsRowBorderInfo.width);
		}
}

/******************************************************************************
 InsertCols

 ******************************************************************************/

void
JTable::InsertCols
	(
	const JIndex		index,
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	if (count == 0)
		{
		return;
		}

	const JSize colCount   = GetColCount();
	const JIndex trueIndex = JMin(index, colCount+1);

	// update aux data (e.g. selection) if we don't have table data

	JTableData::ColsInserted auxMessage(trueIndex, count);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	JCoordinate w = colWidth;
	if (colWidth <= 0 && (trueIndex == 1 || colCount == 0))
		{
		w = itsDefColWidth;
		}
	else if (colWidth <= 0)
		{
		w = itsColWidths->GetElement(trueIndex-1);
		}

	itsColWidths->InsertElementsAtIndex(trueIndex, w, count);

	ColsInserted msg(trueIndex, count, w);

	w = count * w + (count-1) * itsColBorderInfo.width;
	if (colCount > 0)
		{
		w += itsColBorderInfo.width;
		}
	TableWidthChanged(GetColLeft(trueIndex), w);
	TableAdjustBounds(w, 0);
	Broadcast(msg);
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// move the input field

	if (itsIsEditingFlag)
		{
		JPoint newEditCell = itsEditCell;
		auxMessage.AdjustCell(&newEditCell);
		if (newEditCell != itsEditCell)
			{
			itsEditCell = newEditCell;
			MoveInputField(w, 0);
			}
		}
}

/******************************************************************************
 SetColWidth

 ******************************************************************************/

void
JTable::SetColWidth
	(
	const JIndex		index,
	const JCoordinate	colWidth
	)
{
	assert( colWidth > 0 );

	const JCoordinate oldWidth = itsColWidths->GetElement(index);
	if (colWidth != oldWidth)
		{
		itsColWidths->SetElement(index, colWidth);

		const JCoordinate deltaW = colWidth - oldWidth;
		TableWidthChanged(GetColLeft(index) +
						  (deltaW < 0 ? colWidth : oldWidth), deltaW);

		TableAdjustBounds(deltaW, 0);
		Broadcast(ColWidthChanged(index, oldWidth, colWidth));

		// adjust the input field

		if (itsIsEditingFlag && ((JIndex) itsEditCell.x) == index)
			{
			ResizeInputField(colWidth - oldWidth, 0);
			}
		else if (itsIsEditingFlag && ((JIndex) itsEditCell.x) > index)
			{
			MoveInputField(colWidth - oldWidth, 0);
			}
		}
}

/******************************************************************************
 SetAllColWidths

 ******************************************************************************/

void
JTable::SetAllColWidths
	(
	const JCoordinate colWidth
	)
{
	assert( colWidth > 0 );

	const JSize colCount = itsColWidths->GetElementCount();
	if (colCount > 0)
		{
		itsColWidths->RemoveAll();
		itsColWidths->AppendElements(colWidth, colCount);

		const JCoordinate newWidth =
			colCount * colWidth + (colCount-1) * itsColBorderInfo.width;
		TableWidthScaled(newWidth / (JFloat) itsWidth);
		TableSetBounds(newWidth, itsHeight);
		Broadcast(AllColWidthsChanged(colWidth));

		// adjust the input field

		if (itsIsEditingFlag)
			{
			const JRect& editRect = GetCellRect(itsEditCell);
			PlaceInputField(editRect.left, editRect.top);
			SetInputFieldSize(editRect.width(), editRect.height());
			}
		}
}

/******************************************************************************
 RemoveNextCols

 ******************************************************************************/

void
JTable::RemoveNextCols
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	if (firstIndex == 1 && count == GetColCount())
		{
		RemoveAllCols();
		return;
		}

	// update aux data (e.g. selection) if we don't have table data

	JTableData::ColsRemoved auxMessage(firstIndex, count);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	sBorderWidth                   = itsColBorderInfo.width;
	const JCoordinate removedX     = GetColLeft(firstIndex);
	const JCoordinate removedWidth = itsColWidths->SumElements(firstIndex, firstIndex+count-1, GetCellSize);

	itsColWidths->RemoveNextElements(firstIndex, count);
	TableWidthChanged(removedX, -removedWidth);
	TableAdjustBounds(-removedWidth, 0);
	Broadcast(ColsRemoved(firstIndex, count));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// adjust the input field

	if (itsIsEditingFlag)
		{
		JPoint newEditCell = itsEditCell;
		if (!auxMessage.AdjustCell(&newEditCell))
			{
			CancelEditing();
			}
		else if (newEditCell != itsEditCell)
			{
			itsEditCell = newEditCell;
			MoveInputField(-removedWidth, 0);
			}
		}
}

/******************************************************************************
 RemoveAllCols

 ******************************************************************************/

void
JTable::RemoveAllCols()
{
	// update aux data (e.g. selection) if we don't have table data

	JTableData::ColsRemoved auxMessage(1, GetColCount());
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	CancelEditing();

	itsColWidths->RemoveAll();
	itsWidth = 0;
	TableSetBounds(0, itsHeight);
	Broadcast(ColsRemoved(1, auxMessage.GetCount()));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}
}

/******************************************************************************
 MoveCol

 ******************************************************************************/

void
JTable::MoveCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	// update aux data (e.g. selection) if we don't have table data

	JTableData::ColMoved auxMessage(origIndex, newIndex);
	if (itsTableData == NULL)
		{
		Broadcast(PrepareForTableDataMessage(auxMessage));
		}

	TableColMoved(GetColLeft(origIndex),
				  itsColWidths->GetElement(origIndex) + itsColBorderInfo.width,
				  GetColLeft(newIndex));

	itsColWidths->MoveElementToIndex(origIndex, newIndex);
	JRect cellRect(JPoint(origIndex, 1), JPoint(newIndex, GetRowCount()));
	TableRefreshCellRect(cellRect);
	Broadcast(ColMoved(origIndex, newIndex));
	if (itsTableData == NULL)
		{
		Broadcast(auxMessage);
		}

	// adjust the input field

	if (itsIsEditingFlag && ((JIndex) itsEditCell.x) == origIndex)
		{
		itsEditCell.x = newIndex;
		const JRect& editRect = GetCellRect(itsEditCell);
		PlaceInputField(editRect.left, editRect.top);
		}
	else if (itsIsEditingFlag && origIndex < ((JIndex) itsEditCell.x) &&
			 ((JIndex) itsEditCell.x) <= newIndex)
		{
		(itsEditCell.x)--;
		MoveInputField(-GetColWidth(newIndex) - itsColBorderInfo.width, 0);
		}
	else if (itsIsEditingFlag && newIndex <= ((JIndex) itsEditCell.x) &&
			 ((JIndex) itsEditCell.x) < origIndex)
		{
		(itsEditCell.x)++;
		MoveInputField(GetColWidth(newIndex) + itsColBorderInfo.width, 0);
		}
}

/******************************************************************************
 Border info (protected)

 ******************************************************************************/

void
JTable::SetRowBorderInfo
	(
	const JCoordinate	lineWidth,
	const JColorIndex	color
	)
{
	itsRowBorderInfo.color = color;

	assert( lineWidth >= 0 );

	const JCoordinate oldWidth = itsRowBorderInfo.width;
	if (lineWidth != oldWidth)
		{
		itsRowBorderInfo.width = lineWidth;

		const JSize rowCount = GetRowCount();
		if (rowCount > 0)
			{
			TableAdjustBounds(0, (lineWidth - oldWidth) * (rowCount-1));
			}
		TableSetScrollSteps(itsDefColWidth  + itsColBorderInfo.width,
							itsDefRowHeight + itsRowBorderInfo.width);
		Broadcast(RowBorderWidthChanged(oldWidth, lineWidth));

		// adjust the input field

		if (itsIsEditingFlag)
			{
			MoveInputField(0, (lineWidth - oldWidth) * (itsEditCell.y-1));
			}
		}
}

void
JTable::SetColBorderInfo
	(
	const JCoordinate	lineWidth,
	const JColorIndex	color
	)
{
	itsColBorderInfo.color = color;

	assert( lineWidth >= 0 );

	const JCoordinate oldWidth = itsColBorderInfo.width;
	if (lineWidth != oldWidth)
		{
		itsColBorderInfo.width = lineWidth;

		const JSize colCount = GetColCount();
		if (colCount > 0)
			{
			TableAdjustBounds((lineWidth - oldWidth) * (colCount-1), 0);
			}
		TableSetScrollSteps(itsDefColWidth  + itsColBorderInfo.width,
							itsDefRowHeight + itsRowBorderInfo.width);
		Broadcast(ColBorderWidthChanged(oldWidth, lineWidth));

		// adjust the input field

		if (itsIsEditingFlag)
			{
			MoveInputField((lineWidth - oldWidth) * (itsEditCell.x-1), 0);
			}
		}
}

/******************************************************************************
 GetCell

	Returns kJTrue if the specified pt is inside the table's boundaries.

 ******************************************************************************/

JBoolean
JTable::GetCell
	(
	const JPoint&	pt,
	JPoint*			cell
	)
	const
{
	if (pt.x < 0 || pt.x >= itsWidth ||
		pt.y < 0 || pt.y >= itsHeight)
		{
		return kJFalse;
		}

	JIndex row,col;
	if (GetCellIndex(pt.x, *itsColWidths,  itsColBorderInfo.width, &col) &&
		GetCellIndex(pt.y, *itsRowHeights, itsRowBorderInfo.width, &row))
		{
		*cell = JPoint(col,row);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetCellIndex (private)

	Returns kJTrue if the specified pt is inside the table's boundaries.

 ******************************************************************************/

JBoolean
JTable::GetCellIndex
	(
	const JCoordinate		coord,
	JRunArray<JCoordinate>&	lengths,
	const JCoordinate		borderWidth,
	JIndex*					index
	)
	const
{
	if (coord < 0)
		{
		return kJFalse;
		}
	else
		{
		JCoordinate cellLeft;
		sBorderWidth = borderWidth;
		return lengths.FindPositiveSum(coord, 1, index, &cellLeft, GetCellSize);
		}
}

/******************************************************************************
 GetCellRect

 ******************************************************************************/

JRect
JTable::GetCellRect
	(
	const JPoint& cell
	)
	const
{
	JRect cellRect;

	GetCellBoundaries(cell.x, *itsColWidths, itsColBorderInfo.width,
					  &(cellRect.left), &(cellRect.right));
	GetCellBoundaries(cell.y, *itsRowHeights, itsRowBorderInfo.width,
					  &(cellRect.top), &(cellRect.bottom));

	return cellRect;
}

/******************************************************************************
 GetCellBoundaries (private)

 ******************************************************************************/

void
JTable::GetCellBoundaries
	(
	const JIndex			index,
	JRunArray<JCoordinate>&	lengths,
	const JCoordinate		borderWidth,
	JCoordinate*			min,
	JCoordinate*			max
	)
	const
{
	sBorderWidth = borderWidth;
	*min = (index == 1 ? 0 : lengths.SumElements(1, index-1, GetCellSize));
	*max = *min + lengths.GetElement(index);
}

/******************************************************************************
 TableScrollToCell

	If we are a 1D list, then we shouldn't scroll horizontally unless the
	cell would not be visible at all.

 ******************************************************************************/

JBoolean
JTable::TableScrollToCell
	(
	const JPoint&	cell,
	const JBoolean	centerInDisplay
	)
{
	JRect r = GetCellRect(cell);
	if (itsIs1DListFlag &&
		r.left <= TableGetApertureWidth() - GetMin1DVisibleWidth(cell))
		{
		r.left = 0;
		}
	return TableScrollToCellRect(r, centerInDisplay);
}

/******************************************************************************
 GetMin1DVisibleWidth (virtual protected)

	Derived classes should override to return the minimum width that ought
	to be visible when we are acting like a 1D list.

 ******************************************************************************/

JCoordinate
JTable::GetMin1DVisibleWidth
	(
	const JPoint& cell
	)
	const
{
	return 10;
}

/******************************************************************************
 GetEditedCell

 ******************************************************************************/

JBoolean
JTable::GetEditedCell
	(
	JPoint* editCell
	)
	const
{
	if (itsIsEditingFlag)
		{
		*editCell = itsEditCell;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this to specify which cells in the
	table can be edited.

 ******************************************************************************/

JBoolean
JTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 BeginEditing

	Derived class should call this to begin editing a particular cell.
	Note that we do not restrict the editing to be done "in place".
	CreateInputField can activate any sort of input field, even a
	dialog window.

	It is safe to call this function even while something else is
	being edited because it automatically calls EndEditing().

	Not all derived classes will support editing.  The ones that do must
	decide how editing should be triggered and then call these functions
	whenever it is appropriate.

 ******************************************************************************/

JBoolean
JTable::BeginEditing
	(
	const JPoint&	cell,
	const JBoolean	scrollToCell
	)
{
	if (itsIsEditingFlag && cell == itsEditCell)
		{
		if (scrollToCell)
			{
			TableScrollToCell(cell);
			}
		return kJTrue;
		}
	else if (!IsEditable(cell) || !EndEditing())
		{
		// We check IsEditable() first so clicking on a static cell
		// doesn't turn off editing.

		return kJFalse;
		}
	else if (!CreateInputField(cell, GetCellRect(cell)))
		{
		return kJFalse;
		}

	if (scrollToCell)
		{
		TableScrollToCell(cell);
		}
	itsIsEditingFlag = kJTrue;
	itsEditCell      = cell;
	return kJTrue;
}

/******************************************************************************
 ShiftEditing

	Derived class should call this to shift editing to a different cell.
	This will only work if a cell is currently being edited.

 ******************************************************************************/

JBoolean
JTable::ShiftEditing
	(
	const JCoordinate	dx,
	const JCoordinate	dy,
	const JBoolean		scrollToCell
	)
{
	if (!itsIsEditingFlag)
		{
		return kJFalse;
		}

	JPoint newCell = itsEditCell;

	JBoolean search = JI2B( (dx == 0 && (dy == 1 || dy == -1)) ||
							(dy == 0 && (dx == 1 || dx == -1)));

	const JSize rowCount = GetRowCount();
	const JSize colCount = GetColCount();
	do
		{
		newCell += JPoint(dx,dy);

		if (newCell.x < 1)
			{
			newCell.x = 1;
			search    = kJFalse;
			}
		else if (((JIndex) newCell.x) > colCount)
			{
			newCell.x = colCount;
			search    = kJFalse;
			}

		if (newCell.y < 1)
			{
			newCell.y = 1;
			search    = kJFalse;
			}
		else if (((JIndex) newCell.y) > rowCount)
			{
			newCell.y = rowCount;
			search    = kJFalse;
			}
		}
		while (search && !IsEditable(newCell));

	return BeginEditing(newCell, scrollToCell);
}

/******************************************************************************
 EndEditing

	Derived class should call this to safely end editing by validating
	the input and then saving the result.

	This must be called before accessing the underlying JTableData object.

 ******************************************************************************/

JBoolean
JTable::EndEditing()
{
	const JPoint cell = itsEditCell;	// make local copy before calling ExtractInputData()
	if (!itsIsEditingFlag)
		{
		return kJTrue;
		}
	else if (ExtractInputData(cell))
		{
		itsIsEditingFlag = kJFalse;
		DeleteInputField();
		return kJTrue;
		}
	else
		{
		if (itsIsEditingFlag && CellValid(cell))	// safeguard against row being deleted
			{
			TableScrollToCell(cell);
			}
		return kJFalse;
		}
}

/******************************************************************************
 CancelEditing

	Derived class should call this to cancel editing and ignore the input.

 ******************************************************************************/

void
JTable::CancelEditing()
{
	if (itsIsEditingFlag)
		{
		itsIsEditingFlag = kJFalse;
		DeleteInputField();
		}
}

/******************************************************************************
 CreateInputField (virtual protected)

	Derived class must override to create an input field for the
	given cell.  The cell's rect is provided for convenience.
	The derived class must store the object so it can retrieve the
	information when ExtractInputData is called.

	These functions are not pure virtual because not all derived classes will
	support editing, so they should not all be forced to implement these functions.

 ******************************************************************************/

JBoolean
JTable::CreateInputField
	(
	const JPoint&	cell,
	const JRect&	cellRect
	)
{
	assert( 0 /* The programmer forgot to override JTable::CreateInputField() */ );
	return kJFalse;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Derived class must override to extract the information from its active
	input field, check it, and delete the input field if successful.

	Should return kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
JTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( 0 /* The programmer forgot to override JTable::ExtractInputData() */ );
	return kJFalse;
}

/******************************************************************************
 DeleteInputField (virtual protected)

	Derived class must override to remove the active input field.
	The data was already saved when ExtractInputData was called.

 ******************************************************************************/

void
JTable::DeleteInputField()
{
	assert( 0 /* The programmer forgot to override JTable::DeleteInputField() */ );
}

/******************************************************************************
 Adjusting the input field (virtual protected)

	Derived class must override to adjust the position or size of the
	input field.

 ******************************************************************************/

void
JTable::PlaceInputField
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	assert( 0 /* The programmer forgot to override JTable::PlaceInputField() */ );
}

void
JTable::MoveInputField
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	assert( 0 /* The programmer forgot to override JTable::MoveInputField() */ );
}

void
JTable::SetInputFieldSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	assert( 0 /* The programmer forgot to override JTable::SetInputFieldSize() */ );
}

void
JTable::ResizeInputField
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	assert( 0 /* The programmer forgot to override JTable::ResizeInputField() */ );
}

/******************************************************************************
 BeginSelectionDrag (protected)

	We take a cell instead of a point because derived classes may have to
	check the cell before calling us, and calling GetCell() twice is a waste.

 ******************************************************************************/

void
JTable::BeginSelectionDrag
	(
	const JPoint&	cell,
	const JBoolean	extendSelection,
	const JBoolean	selectDiscont
	)
{
	itsSelDragType = kInvalidDrag;

	if (extendSelection)
		{
		if (itsAllowSelectMultipleFlag &&
			itsTableSelection->OKToExtendSelection())
			{
			itsSelDragType = kSelectRangeDrag;
			itsTableSelection->ExtendSelection(cell);
			}
		}
	else if (selectDiscont && itsTableSelection->IsSelected(cell))
		{
		if (!itsAllowSelectMultipleFlag || itsAllowSelectDiscontFlag)
			{
			itsSelDragType = kDeselectCellDrag;
			itsTableSelection->ClearBoat();
			itsTableSelection->ClearAnchor();
			itsTableSelection->SelectCell(cell, kJFalse);
			}
		}
	else if (selectDiscont)
		{
		if (itsAllowSelectMultipleFlag && itsAllowSelectDiscontFlag)
			{
			itsSelDragType = kSelectCellDrag;
			itsTableSelection->SetBoat(cell);
			itsTableSelection->SetAnchor(cell);
			itsTableSelection->SelectCell(cell, kJTrue);
			}
		}
	else
		{
		itsSelDragType =
			itsAllowSelectMultipleFlag ? kSelectRangeDrag : kSelectSingleDrag;
		SelectSingleCell(cell, kJFalse);
		}

	itsPrevSelDragCell = cell;
}

/******************************************************************************
 ContinueSelectionDrag (protected)

	We take a cell instead of a point because derived classes may have to
	check the cell before calling us, and calling GetCell() twice is a waste.

 ******************************************************************************/

void
JTable::ContinueSelectionDrag
	(
	const JPoint& cell
	)
{
	if (itsSelDragType == kInvalidDrag || cell == itsPrevSelDragCell)
		{
		return;
		}

	if (itsSelDragType == kSelectSingleDrag)
		{
		SelectSingleCell(cell, kJFalse);
		}
	else if (itsSelDragType == kSelectCellDrag)
		{
		itsTableSelection->SetBoat(cell);
		itsTableSelection->SetAnchor(cell);
		itsTableSelection->SelectCell(cell, kJTrue);
		}
	else if (itsSelDragType == kDeselectCellDrag)
		{
		itsTableSelection->SelectCell(cell, kJFalse);
		}
	else if (itsSelDragType == kSelectRangeDrag)
		{
		itsTableSelection->ExtendSelection(cell);
		}

	itsPrevSelDragCell = cell;
}

/******************************************************************************
 FinishSelectionDrag (protected)

 ******************************************************************************/

void
JTable::FinishSelectionDrag()
{
	itsSelDragType = kInvalidDrag;
}

/******************************************************************************
 HandleSelectionKeyPress (protected)

	Returns kJTrue if we handled the key.

 ******************************************************************************/

JBoolean
JTable::HandleSelectionKeyPress
	(
	const JCharacter	key,
	const JBoolean		extendSelection
	)
{
	JPoint delta;
	if (key == kJUpArrow)
		{
		delta.Set(0, -1);
		}
	else if (key == kJDownArrow)
		{
		delta.Set(0, +1);
		}
	else if (key == kJLeftArrow)
		{
		delta.Set(-1, 0);
		}
	else if (key == kJRightArrow)
		{
		delta.Set(+1, 0);
		}
	else
		{
		return kJFalse;
		}

	JPoint topSelCell;
	const JBoolean hadSelection =
		itsTableSelection->GetFirstSelectedCell(&topSelCell);

	JPoint cell = topSelCell;
	if (hadSelection && extendSelection && itsAllowSelectMultipleFlag)
		{
		if (itsTableSelection->OKToExtendSelection())
			{
			cell = itsTableSelection->GetBoat();
			if (GetNextSelectableCell(&cell, delta, kJTrue))
				{
				itsTableSelection->ExtendSelection(cell);
				TableScrollToCell(cell);
				}
			else
				{
				TableScrollToCell(itsTableSelection->GetBoat());
				}
			}
		}
	else if (hadSelection && GetNextSelectableCell(&cell, delta, kJFalse))
		{
		SelectSingleCell(cell);
		}
	else if (hadSelection)
		{
		SelectSingleCell(topSelCell);
		}
	else if (GetRowCount() > 0 && GetColCount() > 0)
		{
		SelectSingleCell(JPoint(1,1));
		}

	return kJTrue;
}

/******************************************************************************
 GetNextSelectableCell

	Returns kJFalse if it can't find a selectable cell in the direction delta.

 ******************************************************************************/

JBoolean
JTable::GetNextSelectableCell
	(
	JPoint*			cell,
	const JPoint&	delta,
	const JBoolean	forExtend
	)
	const
{
	if (!CellValid(*cell))
		{
		return kJFalse;
		}

	*cell += delta;
	while (CellValid(*cell) && !IsSelectable(*cell, forExtend))
		{
		*cell += delta;
		}

	return CellValid(*cell);
}

/******************************************************************************
 IsSelectable (virtual)

	The default is for all cells to be selectable.  forExtend is kJTrue if
	the cell will be selected as part of an "extend selection" operation.

 ******************************************************************************/

JBoolean
JTable::IsSelectable
	(
	const JPoint&	cell,
	const JBoolean	forExtend
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 SelectSingleCell

	Selects the specified cell, deselects all other cells, and
	optionally scrolls to make the selected cell visible.

 ******************************************************************************/

void
JTable::SelectSingleCell
	(
	const JPoint&	cell,
	const JBoolean	scroll
	)
{
	itsTableSelection->ClearSelection();

	itsTableSelection->SetBoat(cell);
	itsTableSelection->SetAnchor(cell);
	itsTableSelection->SelectCell(cell);

	if (scroll)
		{
		TableScrollToCell(cell);
		}
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes in our JTableData and JAuxTableData objects.

 ******************************************************************************/

void
JTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	const JBoolean isAuxData = itsAuxDataList->Includes(sender);

	// notify all JAuxTableData objects before updating the table

	if (sender == const_cast<JTableData*>(itsTableData))
		{
		Broadcast(PrepareForTableDataMessage(message));
		}

	// element or aux data changed

	if ((sender == const_cast<JTableData*>(itsTableData) || isAuxData) &&
		message.Is(JTableData::kRectChanged))
		{
		const JTableData::RectChanged* info =
			dynamic_cast(const JTableData::RectChanged*, &message);
		assert( info != NULL );
		const JRect& r = info->GetRect();
		if (sender == const_cast<JTableData*>(itsTableData) &&
			itsIsEditingFlag && r.Contains(itsEditCell))
			{
			CancelEditing();
			}
		TableRefreshCellRect(r);
		}

	// rows changed

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kRowsInserted))
		{
		const JTableData::RowsInserted* info =
			dynamic_cast(const JTableData::RowsInserted*, &message);
		assert( info != NULL );
		InsertRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kRowDuplicated))
		{
		const JTableData::RowDuplicated* info =
			dynamic_cast(const JTableData::RowDuplicated*, &message);
		assert( info != NULL );
		InsertRows(info->GetNewIndex(), 1);
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kRowsRemoved))
		{
		const JTableData::RowsRemoved* info =
			dynamic_cast(const JTableData::RowsRemoved*, &message);
		assert( info != NULL );
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kRowMoved))
		{
		const JTableData::RowMoved* info =
			dynamic_cast(const JTableData::RowMoved*, &message);
		assert( info != NULL );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
		}

	// columns changed

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast(const JTableData::ColsInserted*, &message);
		assert( info != NULL );
		InsertCols(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kColDuplicated))
		{
		const JTableData::ColDuplicated* info =
			dynamic_cast(const JTableData::ColDuplicated*, &message);
		assert( info != NULL );
		InsertCols(info->GetNewIndex(), 1);
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kColsRemoved))
		{
		const JTableData::ColsRemoved* info =
			dynamic_cast(const JTableData::ColsRemoved*, &message);
		assert( info != NULL );
		RemoveNextCols(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JTableData*>(itsTableData) &&
			 message.Is(JTableData::kColMoved))
		{
		const JTableData::ColMoved* info =
			dynamic_cast(const JTableData::ColMoved*, &message);
		assert( info != NULL );
		MoveCol(info->GetOrigIndex(), info->GetNewIndex());
		}

	// something else

	else
		{
		JBroadcaster::Receive(sender, message);
		}

	// update all JAuxTableData objects after updating the table

	if (sender == const_cast<JTableData*>(itsTableData))
		{
		Broadcast(message);
		}
}

/******************************************************************************
 JBroadcaster messages

 ******************************************************************************/

// message types

const JCharacter* JTable::kRowHeightChanged      = "RowHeightChanged::JTable";
const JCharacter* JTable::kAllRowHeightsChanged  = "AllRowHeightsChanged::JTable";
const JCharacter* JTable::kRowsInserted          = "RowsInserted::JTable";
const JCharacter* JTable::kRowsRemoved           = "RowsRemoved::JTable";
const JCharacter* JTable::kRowMoved              = "RowMoved::JTable";
const JCharacter* JTable::kRowBorderWidthChanged = "RowBorderWidthChanged::JTable";

const JCharacter* JTable::kColWidthChanged       = "ColWidthChanged::JTable";
const JCharacter* JTable::kAllColWidthsChanged   = "AllColWidthsChanged::JTable";
const JCharacter* JTable::kColsInserted          = "ColsInserted::JTable";
const JCharacter* JTable::kColsRemoved           = "ColsRemoved::JTable";
const JCharacter* JTable::kColMoved              = "ColMoved::JTable";
const JCharacter* JTable::kColBorderWidthChanged = "ColBorderWidthChanged::JTable";

const JCharacter* JTable::kPrepareForTableDataMessage = "PrepareForTableDataMessage::JTable";
const JCharacter* JTable::kPrepareForTableDataChange  = "PrepareForTableDataChange::JTable";
const JCharacter* JTable::kTableDataChanged           = "TableDataChanged::JTable";

/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JTable::RowsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JTable::RowsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JTable::ColsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JTable::ColsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JTable::RowColMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}
