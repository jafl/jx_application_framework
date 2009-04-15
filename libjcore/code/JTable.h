/******************************************************************************
 JTable.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTable
#define _H_JTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JRunArray.h>
#include <JPtrArray.h>
#include <JRect.h>
#include <jColor.h>

class JTableData;
class JTableSelection;
class JPainter;
class JPagePrinter;
class JEPSPrinter;

class JTable : virtual public JBroadcaster
{
public:

	JTable(const JCoordinate defRowHeight, const JCoordinate defColWidth,
		   const JColorIndex borderColor, const JColorIndex selectionColor);

	virtual ~JTable();

	void	SetTableData(const JTableData* data);

	JSize		GetRowCount() const;
	JCoordinate	GetRowHeight(const JIndex index) const;
	void		SetRowHeight(const JIndex index, const JCoordinate rowHeight);
	void		SetAllRowHeights(const JCoordinate rowHeight);
	JCoordinate	GetDefaultRowHeight() const;
	void		SetDefaultRowHeight(const JCoordinate defRowHeight);

	JSize		GetColCount() const;
	JCoordinate	GetColWidth(const JIndex index) const;
	void		SetColWidth(const JIndex index, const JCoordinate colWidth);
	void		SetAllColWidths(const JCoordinate colWidth);
	JCoordinate	GetDefaultColWidth() const;
	void		SetDefaultColWidth(const JCoordinate defColWidth);

	JBoolean	GetCell(const JPoint& pt, JPoint* cell) const;
	JRect		GetCellRect(const JPoint& cell) const;
	JCoordinate	GetRowTop(const JIndex index) const;
	JCoordinate	GetColLeft(const JIndex index) const;

	JTableSelection&		GetTableSelection();
	const JTableSelection&	GetTableSelection() const;

	virtual JBoolean	IsSelectable(const JPoint& cell,
									 const JBoolean forExtend) const;
	void				SelectSingleCell(const JPoint& cell,
										 const JBoolean scroll = kJTrue);
	JBoolean			GetNextSelectableCell(JPoint* cell, const JPoint& delta,
											  const JBoolean forExtend) const;

	virtual JBoolean	IsEditable(const JPoint& cell) const;
	JBoolean			BeginEditing(const JPoint& cell, const JBoolean scrollToCell = kJTrue);
	JBoolean			ShiftEditing(const JCoordinate dx, const JCoordinate dy,
									 const JBoolean scrollToCell = kJTrue);
	JBoolean			EndEditing();
	void				CancelEditing();

	JBoolean	IsEditing() const;
	JBoolean	GetEditedCell(JPoint* editCell) const;

	void	Print(JPagePrinter& p,
				  const JBoolean printRowHeader = kJTrue,
				  const JBoolean printColHeader = kJTrue);
	void	Print(JEPSPrinter& p,
				  const JBoolean printRowHeader = kJTrue,
				  const JBoolean printColHeader = kJTrue);
	void	DrawForPrint(JPainter& p, const JPoint& topLeft,
						 const JBoolean userPrintRowHeader,
						 const JBoolean userPrintColHeader);

	void	GetRowBorderInfo(JCoordinate* lineWidth, JColorIndex* color) const;
	void	SetRowBorderInfo(const JCoordinate lineWidth, const JColorIndex color);

	void	GetColBorderInfo(JCoordinate* lineWidth, JColorIndex* color) const;
	void	SetColBorderInfo(const JCoordinate lineWidth, const JColorIndex color);

	JColorIndex	GetSelectionColor() const;
	void		SetSelectionColor(const JColorIndex color);

	JBoolean	GetRowHeader(const JTable** table) const;
	void		SetRowHeader(JTable* table);
	JBoolean	GetColHeader(const JTable** table) const;
	void		SetColHeader(JTable* table);

	JBoolean	RowIndexValid(const JIndex index) const;
	JBoolean	ColIndexValid(const JIndex index) const;
	JBoolean	CellValid(const JPoint& cell) const;

	JBoolean	TableScrollToCell(const JPoint& cell,
								  const JBoolean centerInDisplay = kJFalse);

	// automatically called by JAuxTableData

	void	RegisterAuxData(JTableData* auxData);
	void	RemoveAuxData(JTableData* auxData);

protected:

	enum DrawOrder
	{
		kDrawByRow,
		kDrawByCol
	};

protected:

	virtual void	TableRefresh() = 0;
	virtual void	TableRefreshRect(const JRect& rect) = 0;
	void			TableRefreshCell(const JPoint& cell);
	void			TableRefreshCell(const JIndex row, const JIndex col);
	void			TableRefreshCellRect(const JRect& cellRect);
	void			TableRefreshRow(const JIndex index);
	void			TableRefreshCol(const JIndex index);

	JBoolean		WillActLike1DList() const;
	void			ShouldActLike1DList(const JBoolean act1D = kJTrue);

	void			SetDrawOrder(const DrawOrder order);
	void			TableDraw(JPainter& p, const JRect& r,
							  const JBoolean drawLowerRightBorder = kJFalse);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) = 0;
	virtual void	TablePrepareToDrawRow(const JIndex rowIndex,
										  const JIndex firstCol, const JIndex lastCol);
	virtual void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow);
	JBoolean		Paginate(const JCoordinate pageWidth, const JCoordinate pageHeight,
							 const JBoolean userPrintRowHeader,
							 JArray<JCoordinate>* rowBreakpts, JBoolean* printRowHeader,
							 const JBoolean userPrintColHeader,
							 JArray<JCoordinate>* colBreakpts, JBoolean* printColHeader) const;
	void			HilightIfSelected(JPainter& p, const JPoint& cell,
									  const JRect& rect) const;

	void	InsertRows(const JIndex index, const JSize count, const JCoordinate rowHeight = 0);
	void	PrependRows(const JSize count, const JCoordinate rowHeight = 0);
	void	AppendRows(const JSize count, const JCoordinate rowHeight = 0);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count, const JCoordinate colWidth = 0);
	void	PrependCols(const JSize count, const JCoordinate colWidth = 0);
	void	AppendCols(const JSize count, const JCoordinate colWidth = 0);
	void	RemoveCol(const JIndex index);
	void	RemoveNextCols(const JIndex firstIndex, const JSize count);
	void	RemovePrevCols(const JIndex lastIndex, const JSize count);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	virtual void		TableSetGUIBounds(const JCoordinate w, const JCoordinate h) = 0;
	virtual void		TableSetScrollSteps(const JCoordinate hStep,
											const JCoordinate vStep) = 0;
	virtual void		TableHeightChanged(const JCoordinate y, const JCoordinate delta) = 0;
	virtual void		TableHeightScaled(const JFloat scaleFactor) = 0;
	virtual void		TableRowMoved(const JCoordinate origY, const JSize height,
									  const JCoordinate newY) = 0;
	virtual void		TableWidthChanged(const JCoordinate x, const JCoordinate delta) = 0;
	virtual void		TableWidthScaled(const JFloat scaleFactor) = 0;
	virtual void		TableColMoved(const JCoordinate origX, const JSize width,
									  const JCoordinate newX) = 0;
	virtual JBoolean	TableScrollToCellRect(const JRect& cellRect,
											  const JBoolean centerInDisplay = kJFalse) = 0;
	virtual JCoordinate	TableGetApertureWidth() const = 0;
	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const;

	virtual JBoolean	CreateInputField(const JPoint& cell, const JRect& cellRect);
	virtual JBoolean	ExtractInputData(const JPoint& cell);
	virtual void		DeleteInputField();
	virtual void		PlaceInputField(const JCoordinate x, const JCoordinate y);
	virtual void		MoveInputField(const JCoordinate dx, const JCoordinate dy);
	virtual void		SetInputFieldSize(const JCoordinate w, const JCoordinate h);
	virtual void		ResizeInputField(const JCoordinate dw, const JCoordinate dh);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	JBoolean	IsDraggingSelection() const;
	void		SetSelectionBehavior(const JBoolean allowMultiple,
									 const JBoolean allowDiscont);
	void		BeginSelectionDrag(const JPoint& cell, const JBoolean extendSelection,
								   const JBoolean selectDiscont);
	void		ContinueSelectionDrag(const JPoint& cell);
	void		FinishSelectionDrag();
	JBoolean	HandleSelectionKeyPress(const JCharacter key,
										const JBoolean extendSelection);

	void	AdjustToTableData();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct BorderInfo
	{
		JCoordinate	width;
		JColorIndex	color;

		BorderInfo(const JCoordinate w, const JColorIndex c)
			:
			width(w), color(c)
		{ };
	};

	enum SelDragType
	{
		kInvalidDrag,
		kSelectSingleDrag,
		kSelectCellDrag,
		kDeselectCellDrag,
		kSelectRangeDrag
	};

private:

	DrawOrder	itsDrawOrder;
	JBoolean	itsIs1DListFlag;

	JCoordinate	itsWidth;
	JCoordinate	itsHeight;

	JCoordinate	itsDefRowHeight;
	JCoordinate	itsDefColWidth;

	JRunArray<JCoordinate>*	itsRowHeights;
	JRunArray<JCoordinate>*	itsColWidths;

	const JTableData*			itsTableData;
	JTableSelection*			itsTableSelection;
	JPtrArray<JBroadcaster>*	itsAuxDataList;		// we don't own the objects in the list

	BorderInfo	itsRowBorderInfo;
	BorderInfo	itsColBorderInfo;
	JColorIndex	itsSelectionColor;

	// used when printing

	JTable* itsRowHdrTable;
	JTable*	itsColHdrTable;

	// used while editing contents of a cell

	JBoolean	itsIsEditingFlag;
	JPoint		itsEditCell;

	// used while dragging selection

	SelDragType	itsSelDragType;
	JPoint		itsPrevSelDragCell;
	JBoolean	itsAllowSelectMultipleFlag;
	JBoolean	itsAllowSelectDiscontFlag;

private:

	JBoolean	GetVisibleRange(const JCoordinate min, const JCoordinate max,
								JRunArray<JCoordinate>& lengths,
								const JCoordinate borderWidth,
								JIndex* firstIndex, JIndex* lastIndex) const;
	JBoolean	Paginate(const JCoordinate stripLength,
						 JRunArray<JCoordinate>& lengths,
						 const JCoordinate borderWidth,
						 JArray<JCoordinate>* breakpts) const;
	void		PrintPage(JPainter& p, const JPoint& topLeft, const JPoint& botRight,
						  const JPoint& offset, const JBoolean drawFrame);
	void		PrintEPS1(JPainter& p, const JPoint& topLeft,
						  const JBoolean printRowHeader,
						  const JBoolean printColHeader);

	JBoolean	GetCellIndex(const JCoordinate coord, JRunArray<JCoordinate>& lengths,
							 const JCoordinate borderWidth, JIndex* index) const;
	void		GetCellBoundaries(const JIndex index, JRunArray<JCoordinate>& lengths,
								  const JCoordinate borderWidth,
								  JCoordinate* min, JCoordinate* max) const;

	void	TableDrawRowBorders(JPainter& p, const JRect& r,
								const JIndex firstRow, const JIndex lastRow,
								const JBoolean drawBottomBorder);
	void	TableDrawColBorders(JPainter& p, const JRect& r,
								const JIndex firstCol, const JIndex lastCol,
								const JBoolean drawRightBorder);

	void	TableSetBounds(const JCoordinate w, const JCoordinate h);
	void	TableAdjustBounds(const JCoordinate dw, const JCoordinate dh);

	static JInteger	GetCellSize(const JCoordinate& value);

	// not allowed

	JTable(const JTable& source);
	const JTable& operator=(const JTable& source);

public:

	// JBroadcaster messages

	static const JCharacter* kRowHeightChanged;
	static const JCharacter* kAllRowHeightsChanged;
	static const JCharacter* kRowsInserted;
	static const JCharacter* kRowsRemoved;
	static const JCharacter* kRowMoved;
	static const JCharacter* kRowBorderWidthChanged;

	static const JCharacter* kColWidthChanged;
	static const JCharacter* kAllColWidthsChanged;
	static const JCharacter* kColsInserted;
	static const JCharacter* kColsRemoved;
	static const JCharacter* kColMoved;
	static const JCharacter* kColBorderWidthChanged;

	static const JCharacter* kPrepareForTableDataMessage;	// message from JTableData
	static const JCharacter* kPrepareForTableDataChange;	// about to switch to different JTableData
	static const JCharacter* kTableDataChanged;				// now using different JTableData

protected:

	// base classes for JBroadcaster messages

	class RowColMessage : public JBroadcaster::Message
		{
		public:

			RowColMessage(const JCharacter* type,
						  const JIndex firstIndex, const JSize count)
				:
				JBroadcaster::Message(type),
				itsFirstIndex(firstIndex),
				itsCount(count)
				{ };

			JIndex
			GetFirstIndex() const
			{
				return itsFirstIndex;
			};

			JIndex
			GetLastIndex() const
			{
				return itsFirstIndex + itsCount-1;
			};

			JSize
			GetCount() const
			{
				return itsCount;
			};

			JBoolean
			Contains(const JIndex index) const
			{
				return JConvertToBoolean(
						GetFirstIndex() <= index && index <= GetLastIndex() );
			};

		private:

			JIndex	itsFirstIndex;
			JSize	itsCount;
		};

	class RowColMoved : public JBroadcaster::Message
		{
		public:

			RowColMoved(const JCharacter* type,
						const JIndex origIndex, const JIndex newIndex)
				:
				JBroadcaster::Message(type),
				itsOrigIndex(origIndex),
				itsNewIndex(newIndex)
				{ };

			void	AdjustIndex(JIndex* index) const;

			JIndex
			GetOrigIndex() const
			{
				return itsOrigIndex;
			};

			JIndex
			GetNewIndex() const
			{
				return itsNewIndex;
			};

		private:

			JIndex	itsOrigIndex;
			JIndex	itsNewIndex;
		};

	class BorderWidthChanged : public JBroadcaster::Message
		{
		public:

			BorderWidthChanged(const JCharacter* type,
							   const JCoordinate origBorderWidth,
							   const JCoordinate newBorderWidth)
				:
				JBroadcaster::Message(type),
				itsOrigWidth(origBorderWidth),
				itsNewWidth(newBorderWidth)
				{ };

			JCoordinate
			GetOrigBorderWidth() const
			{
				return itsOrigWidth;
			};

			JCoordinate
			GetNewBorderWidth() const
			{
				return itsNewWidth;
			};

		private:

			JCoordinate	itsOrigWidth;
			JCoordinate	itsNewWidth;
		};

public:

	// row messages

	class RowHeightChanged : public JBroadcaster::Message
		{
		public:

			RowHeightChanged(const JIndex index, const JCoordinate origRowHeight,
							 const JCoordinate newRowHeight)
				:
				JBroadcaster::Message(kRowHeightChanged),
				itsIndex(index),
				itsOrigRowHeight(origRowHeight),
				itsNewRowHeight(newRowHeight)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

			JCoordinate
			GetOrigRowHeight() const
			{
				return itsOrigRowHeight;
			};

			JCoordinate
			GetNewRowHeight() const
			{
				return itsNewRowHeight;
			};

		private:

			JIndex		itsIndex;
			JCoordinate	itsOrigRowHeight;
			JCoordinate	itsNewRowHeight;
		};

	class AllRowHeightsChanged : public JBroadcaster::Message
		{
		public:

			AllRowHeightsChanged(const JCoordinate rowHeight)
				:
				JBroadcaster::Message(kAllRowHeightsChanged),
				itsRowHeight(rowHeight)
				{ };

			JCoordinate
			GetNewRowHeight() const
			{
				return itsRowHeight;
			};

		private:

			JCoordinate itsRowHeight;
		};

	class RowsInserted : public RowColMessage
		{
		public:

			RowsInserted(const JIndex firstIndex, const JSize count,
						 const JCoordinate rowHeight)
				:
				RowColMessage(kRowsInserted, firstIndex, count),
				itsRowHeight(rowHeight)
				{ };

			void	AdjustIndex(JIndex* index) const;

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex = cell->y;
				AdjustIndex(&rowIndex);
				cell->y = rowIndex;
			};

			JCoordinate
			GetRowHeight() const
			{
				return itsRowHeight;
			};

		private:

			JCoordinate	itsRowHeight;
		};

	class RowsRemoved : public RowColMessage
		{
		public:

			RowsRemoved(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kRowsRemoved, firstIndex, count)
				{ };

			JBoolean	AdjustIndex(JIndex* index) const;

			JBoolean
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex   = cell->y;
				const JBoolean ok = AdjustIndex(&rowIndex);
				cell->y           = rowIndex;
				return ok;
			};
		};

	class RowMoved : public RowColMoved
		{
		public:

			RowMoved(const JIndex origIndex, const JIndex newIndex)
				:
				RowColMoved(kRowMoved, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex = cell->y;
				AdjustIndex(&rowIndex);
				cell->y = rowIndex;
			};
		};

	class RowBorderWidthChanged : public BorderWidthChanged
		{
		public:

			RowBorderWidthChanged(const JCoordinate origBorderWidth,
								  const JCoordinate newBorderWidth)
				:
				BorderWidthChanged(kRowBorderWidthChanged,
								   origBorderWidth, newBorderWidth)
				{ };
		};

	// column messages

	class ColWidthChanged : public JBroadcaster::Message
		{
		public:

			ColWidthChanged(const JIndex index, const JCoordinate origColWidth,
							const JCoordinate newColWidth)
				:
				JBroadcaster::Message(kColWidthChanged),
				itsIndex(index),
				itsOrigColWidth(origColWidth),
				itsNewColWidth(newColWidth)
				{ };

			JIndex
			GetIndex() const
			{
				return itsIndex;
			};

			JCoordinate
			GetOrigColWidth() const
			{
				return itsOrigColWidth;
			};

			JCoordinate
			GetNewColWidth() const
			{
				return itsNewColWidth;
			};

		private:

			JIndex		itsIndex;
			JCoordinate	itsOrigColWidth;
			JCoordinate	itsNewColWidth;
		};

	class AllColWidthsChanged : public JBroadcaster::Message
		{
		public:

			AllColWidthsChanged(const JCoordinate colWidth)
				:
				JBroadcaster::Message(kAllColWidthsChanged),
				itsColWidth(colWidth)
				{ };

			JCoordinate
			GetNewColWidth() const
			{
				return itsColWidth;
			};

		private:

			JCoordinate itsColWidth;
		};

	class ColsInserted : public RowColMessage
		{
		public:

			ColsInserted(const JIndex firstIndex, const JSize count,
						 const JCoordinate colWidth)
				:
				RowColMessage(kColsInserted, firstIndex, count),
				itsColWidth(colWidth)
				{ };

			void	AdjustIndex(JIndex* index) const;

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex = cell->x;
				AdjustIndex(&colIndex);
				cell->x = colIndex;
			};

			JCoordinate
			GetColWidth() const
			{
				return itsColWidth;
			};

		private:

			JCoordinate	itsColWidth;
		};

	class ColsRemoved : public RowColMessage
		{
		public:

			ColsRemoved(const JIndex firstIndex, const JSize count)
				:
				RowColMessage(kColsRemoved, firstIndex, count)
				{ };

			JBoolean	AdjustIndex(JIndex* index) const;

			JBoolean
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex   = cell->x;
				const JBoolean ok = AdjustIndex(&colIndex);
				cell->x           = colIndex;
				return ok;
			};
		};

	class ColMoved : public RowColMoved
		{
		public:

			ColMoved(const JIndex origIndex, const JIndex newIndex)
				:
				RowColMoved(kColMoved, origIndex, newIndex)
				{ };

			void
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex = cell->x;
				AdjustIndex(&colIndex);
				cell->x = colIndex;
			};
		};

	class ColBorderWidthChanged : public BorderWidthChanged
		{
		public:

			ColBorderWidthChanged(const JCoordinate origBorderWidth,
								  const JCoordinate newBorderWidth)
				:
				BorderWidthChanged(kColBorderWidthChanged,
								   origBorderWidth, newBorderWidth)
				{ };
		};

	// other messages

	class PrepareForTableDataMessage : public JBroadcaster::Message
		{
		public:

			PrepareForTableDataMessage(const JBroadcaster::Message& message)
				:
				JBroadcaster::Message(kPrepareForTableDataMessage),
				itsMessage(message)
				{ };

			const JBroadcaster::Message&
			GetMessage() const
			{
				return itsMessage;
			};

		private:

			const JBroadcaster::Message&	itsMessage;
		};

	class PrepareForTableDataChange : public JBroadcaster::Message
		{
		public:

			PrepareForTableDataChange()
				:
				JBroadcaster::Message(kPrepareForTableDataChange)
				{ };
		};

	class TableDataChanged : public JBroadcaster::Message
		{
		public:

			TableDataChanged()
				:
				JBroadcaster::Message(kTableDataChanged)
				{ };
		};
};


/******************************************************************************
 GetRowCount

 ******************************************************************************/

inline JSize
JTable::GetRowCount()
	const
{
	return itsRowHeights->GetElementCount();
}

/******************************************************************************
 RowIndexValid

 ******************************************************************************/

inline JBoolean
JTable::RowIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= GetRowCount() );
}

/******************************************************************************
 GetRowTop

 ******************************************************************************/

inline JCoordinate
JTable::GetRowTop
	(
	const JIndex index
	)
	const
{
	JCoordinate min, max;
	GetCellBoundaries(index, *itsRowHeights, itsRowBorderInfo.width, &min, &max);
	return min;
}

/******************************************************************************
 GetRowHeight

 ******************************************************************************/

inline JCoordinate
JTable::GetRowHeight
	(
	const JIndex index
	)
	const
{
	return itsRowHeights->GetElement(index);
}

/******************************************************************************
 GetDefaultRowHeight

 ******************************************************************************/

inline JCoordinate
JTable::GetDefaultRowHeight()
	const
{
	return itsDefRowHeight;
}

/******************************************************************************
 Row manipulations

 ******************************************************************************/

inline void
JTable::PrependRows
	(
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	InsertRows(1, count, rowHeight);
}

inline void
JTable::AppendRows
	(
	const JSize			count,
	const JCoordinate	rowHeight
	)
{
	InsertRows(GetRowCount()+1, count, rowHeight);
}

inline void
JTable::RemoveRow
	(
	const JIndex index
	)
{
	RemoveNextRows(index, 1);
}

inline void
JTable::RemovePrevRows
	(
	const JIndex	lastIndex,
	const JSize		count
	)
{
	RemoveNextRows(lastIndex-count+1, count);
}

/******************************************************************************
 GetColCount

 ******************************************************************************/

inline JSize
JTable::GetColCount()
	const
{
	return itsColWidths->GetElementCount();
}

/******************************************************************************
 ColIndexValid

 ******************************************************************************/

inline JBoolean
JTable::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= GetColCount() );
}

/******************************************************************************
 GetColLeft

 ******************************************************************************/

inline JCoordinate
JTable::GetColLeft
	(
	const JIndex index
	)
	const
{
	JCoordinate min, max;
	GetCellBoundaries(index, *itsColWidths, itsColBorderInfo.width, &min, &max);
	return min;
}

/******************************************************************************
 GetColWidth

 ******************************************************************************/

inline JCoordinate
JTable::GetColWidth
	(
	const JIndex index
	)
	const
{
	return itsColWidths->GetElement(index);
}

/******************************************************************************
 GetDefaultColWidth

 ******************************************************************************/

inline JCoordinate
JTable::GetDefaultColWidth()
	const
{
	return itsDefColWidth;
}

/******************************************************************************
 Column manipulations

 ******************************************************************************/

inline void
JTable::PrependCols
	(
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	InsertCols(1, count, colWidth);
}

inline void
JTable::AppendCols
	(
	const JSize			count,
	const JCoordinate	colWidth
	)
{
	InsertCols(GetColCount()+1, count, colWidth);
}

inline void
JTable::RemoveCol
	(
	const JIndex index
	)
{
	RemoveNextCols(index, 1);
}

inline void
JTable::RemovePrevCols
	(
	const JIndex	lastIndex,
	const JSize		count
	)
{
	RemoveNextCols(lastIndex-count+1, count);
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline JBoolean
JTable::CellValid
	(
	const JPoint& cell
	)
	const
{
	return JConvertToBoolean( RowIndexValid(cell.y) && ColIndexValid(cell.x) );
}

/******************************************************************************
 Act like 1D list (protected)

 ******************************************************************************/

inline JBoolean
JTable::WillActLike1DList()
	const
{
	return itsIs1DListFlag;
}

inline void
JTable::ShouldActLike1DList
	(
	const JBoolean act1D
	)
{
	itsIs1DListFlag = act1D;
}

/******************************************************************************
 SetDrawOrder (protected)

 ******************************************************************************/

inline void
JTable::SetDrawOrder
	(
	const DrawOrder order
	)
{
	itsDrawOrder = order;
}

/******************************************************************************
 Default sizes

 ******************************************************************************/

inline void
JTable::SetDefaultRowHeight
	(
	const JCoordinate defRowHeight
	)
{
	itsDefRowHeight = defRowHeight;
	TableSetScrollSteps(itsDefColWidth  + itsColBorderInfo.width,
						itsDefRowHeight + itsRowBorderInfo.width);
}

inline void
JTable::SetDefaultColWidth
	(
	const JCoordinate defColWidth
	)
{
	itsDefColWidth = defColWidth;
	TableSetScrollSteps(itsDefColWidth  + itsColBorderInfo.width,
						itsDefRowHeight + itsRowBorderInfo.width);
}

/******************************************************************************
 Border info (protected)

 ******************************************************************************/

inline void
JTable::GetRowBorderInfo
	(
	JCoordinate*	lineWidth,
	JColorIndex*	color
	)
	const
{
	*lineWidth = itsRowBorderInfo.width;
	*color     = itsRowBorderInfo.color;
}

inline void
JTable::GetColBorderInfo
	(
	JCoordinate*	lineWidth,
	JColorIndex*	color
	)
	const
{
	*lineWidth = itsColBorderInfo.width;
	*color     = itsColBorderInfo.color;
}

/******************************************************************************
 Table bounds (private)

 ******************************************************************************/

inline void
JTable::TableSetBounds
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsWidth  = w;
	itsHeight = h;
	TableSetGUIBounds(itsWidth, itsHeight);
}

inline void
JTable::TableAdjustBounds
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	itsWidth  += dw;
	itsHeight += dh;
	TableSetGUIBounds(itsWidth, itsHeight);
}

/******************************************************************************
 Refresh (protected)

 ******************************************************************************/

inline void
JTable::TableRefreshCell
	(
	const JPoint& cell
	)
{
	TableRefreshRect(GetCellRect(cell));
}

inline void
JTable::TableRefreshCell
	(
	const JIndex row,
	const JIndex col
	)
{
	TableRefreshRect(GetCellRect(JPoint(col,row)));
}

inline void
JTable::TableRefreshRow
	(
	const JIndex index
	)
{
	TableRefreshCellRect(JRect(index, 1, index+1, GetColCount()+1));
}

inline void
JTable::TableRefreshCol
	(
	const JIndex index
	)
{
	TableRefreshCellRect(JRect(1, index, GetRowCount()+1, index+1));
}

/******************************************************************************
 Row and column headers (protected)

 ******************************************************************************/

inline JBoolean
JTable::GetRowHeader
	(
	const JTable** table
	)
	const
{
	*table = itsRowHdrTable;
	return JI2B( itsRowHdrTable != NULL );
}

inline void
JTable::SetRowHeader
	(
	JTable* table
	)
{
	itsRowHdrTable = table;
}

inline JBoolean
JTable::GetColHeader
	(
	const JTable** table
	)
	const
{
	*table = itsColHdrTable;
	return JI2B( itsColHdrTable != NULL );
}

inline void
JTable::SetColHeader
	(
	JTable* table
	)
{
	itsColHdrTable = table;
}

/******************************************************************************
 GetTableSelection

 ******************************************************************************/

inline JTableSelection&
JTable::GetTableSelection()
{
	return *itsTableSelection;
}

inline const JTableSelection&
JTable::GetTableSelection()
	const
{
	return *itsTableSelection;
}

/******************************************************************************
 Selection color

 ******************************************************************************/

inline JColorIndex
JTable::GetSelectionColor()
	const
{
	return itsSelectionColor;
}

inline void
JTable::SetSelectionColor
	(
	const JColorIndex color
	)
{
	itsSelectionColor = color;
	TableRefresh();
}

/******************************************************************************
 IsEditing

 ******************************************************************************/

inline JBoolean
JTable::IsEditing()
	const
{
	return itsIsEditingFlag;
}

/******************************************************************************
 IsDraggingSelection

 ******************************************************************************/

inline JBoolean
JTable::IsDraggingSelection()
	const
{
	return JI2B( itsSelDragType != kInvalidDrag );
}

/******************************************************************************
 SetSelectionBehavior (protected)

 ******************************************************************************/

inline void
JTable::SetSelectionBehavior
	(
	const JBoolean allowMultiple,
	const JBoolean allowDiscont
	)
{
	itsAllowSelectMultipleFlag = allowMultiple;
	itsAllowSelectDiscontFlag  = allowDiscont;
}

#endif
