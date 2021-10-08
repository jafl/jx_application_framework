/******************************************************************************
 JTable.h

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTable
#define _H_JTable

#include "jx-af/jcore/JRunArray.h"
#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JRect.h"
#include "jx-af/jcore/jColor.h"

class JTableData;
class JTableSelection;
class JPainter;
class JPagePrinter;
class JEPSPrinter;

class JTable : virtual public JBroadcaster
{
public:

	JTable(const JCoordinate defRowHeight, const JCoordinate defColWidth,
		   const JColorID borderColor, const JColorID selectionColor);

	~JTable() override;

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

	bool		GetCell(const JPoint& pt, JPoint* cell) const;
	JRect		GetCellRect(const JPoint& cell) const;
	JCoordinate	GetRowTop(const JIndex index) const;
	JCoordinate	GetColLeft(const JIndex index) const;

	JTableSelection&		GetTableSelection();
	const JTableSelection&	GetTableSelection() const;

	virtual bool	IsSelectable(const JPoint& cell,
								 const bool forExtend) const;
	void			SelectSingleCell(const JPoint& cell,
									 const bool scroll = true);
	bool			GetNextSelectableCell(JPoint* cell, const JPoint& delta,
										  const bool forExtend) const;

	virtual bool	IsEditable(const JPoint& cell) const;
	bool			BeginEditing(const JPoint& cell, const bool scrollToCell = true);
	bool			ShiftEditing(const JCoordinate dx, const JCoordinate dy,
								 const bool scrollToCell = true);
	bool	EndEditing();
	void	CancelEditing();

	bool	IsEditing() const;
	bool	GetEditedCell(JPoint* editCell) const;

	void	Print(JPagePrinter& p,
				  const bool printRowHeader = true,
				  const bool printColHeader = true);
	void	Print(JEPSPrinter& p,
				  const bool printRowHeader = true,
				  const bool printColHeader = true);
	void	DrawForPrint(JPainter& p, const JPoint& topLeft,
						 const bool userPrintRowHeader,
						 const bool userPrintColHeader);

	void	GetRowBorderInfo(JCoordinate* lineWidth, JColorID* color) const;
	void	SetRowBorderInfo(const JCoordinate lineWidth, const JColorID color);

	void	GetColBorderInfo(JCoordinate* lineWidth, JColorID* color) const;
	void	SetColBorderInfo(const JCoordinate lineWidth, const JColorID color);

	JColorID	GetSelectionColor() const;
	void		SetSelectionColor(const JColorID color);

	bool	GetRowHeader(const JTable** table) const;
	void	SetRowHeader(JTable* table);
	bool	GetColHeader(const JTable** table) const;
	void	SetColHeader(JTable* table);

	bool	RowIndexValid(const JIndex index) const;
	bool	ColIndexValid(const JIndex index) const;
	bool	CellValid(const JPoint& cell) const;

	bool	TableScrollToCell(const JPoint& cell,
							  const bool centerInDisplay = false);

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

	bool			WillActLike1DList() const;
	void			ShouldActLike1DList(const bool act1D = true);

	void			SetDrawOrder(const DrawOrder order);
	void			TableDraw(JPainter& p, const JRect& r,
							  const bool drawLowerRightBorder = false);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) = 0;
	virtual void	TablePrepareToDrawRow(const JIndex rowIndex,
										  const JIndex firstCol, const JIndex lastCol);
	virtual void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow);
	bool			Paginate(const JCoordinate pageWidth, const JCoordinate pageHeight,
							 const bool userPrintRowHeader,
							 JArray<JCoordinate>* rowBreakpts, bool* printRowHeader,
							 const bool userPrintColHeader,
							 JArray<JCoordinate>* colBreakpts, bool* printColHeader) const;
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
	virtual bool		TableScrollToCellRect(const JRect& cellRect,
											  const bool centerInDisplay = false) = 0;
	virtual JCoordinate	TableGetApertureWidth() const = 0;
	virtual JCoordinate	GetMin1DVisibleWidth(const JPoint& cell) const;

	virtual bool		CreateInputField(const JPoint& cell, const JRect& cellRect);
	virtual bool		ExtractInputData(const JPoint& cell);
	virtual void		DeleteInputField();
	virtual void		PlaceInputField(const JCoordinate x, const JCoordinate y);
	virtual void		MoveInputField(const JCoordinate dx, const JCoordinate dy);
	virtual void		SetInputFieldSize(const JCoordinate w, const JCoordinate h);
	virtual void		ResizeInputField(const JCoordinate dw, const JCoordinate dh);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	bool	IsDraggingSelection() const;
	void	SetSelectionBehavior(const bool allowMultiple,
								 const bool allowDiscont);
	void	BeginSelectionDrag(const JPoint& cell, const bool extendSelection,
							   const bool selectDiscont);
	void	ContinueSelectionDrag(const JPoint& cell);
	void	FinishSelectionDrag();
	bool	HandleSelectionKeyPress(const JUtf8Character& key,
									const bool extendSelection);

	void	AdjustToTableData();

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	struct BorderInfo
	{
		JCoordinate	width;
		JColorID	color;

		BorderInfo(const JCoordinate w, const JColorID c)
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
	bool		itsIs1DListFlag;

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
	JColorID	itsSelectionColor;

	// used when printing

	JTable* itsRowHdrTable;
	JTable*	itsColHdrTable;

	// used while editing contents of a cell

	bool	itsIsEditingFlag;
	JPoint	itsEditCell;

	// used while dragging selection

	SelDragType	itsSelDragType;
	JPoint		itsPrevSelDragCell;
	bool		itsAllowSelectMultipleFlag;
	bool		itsAllowSelectDiscontFlag;

private:

	bool	GetVisibleRange(const JCoordinate min, const JCoordinate max,
								JRunArray<JCoordinate>& lengths,
								const JCoordinate borderWidth,
								JIndex* firstIndex, JIndex* lastIndex) const;
	bool	Paginate(const JCoordinate stripLength,
					 JRunArray<JCoordinate>& lengths,
					 const JCoordinate borderWidth,
					 JArray<JCoordinate>* breakpts) const;
	void	PrintPage(JPainter& p, const JPoint& topLeft, const JPoint& botRight,
					  const JPoint& offset, const bool drawFrame);
	void	PrintEPS1(JPainter& p, const JPoint& topLeft,
					  const bool printRowHeader,
					  const bool printColHeader);

	bool	GetCellIndex(const JCoordinate coord, JRunArray<JCoordinate>& lengths,
						 const JCoordinate borderWidth, JIndex* index) const;
	void	GetCellBoundaries(const JIndex index, JRunArray<JCoordinate>& lengths,
							  const JCoordinate borderWidth,
							  JCoordinate* min, JCoordinate* max) const;

	void	TableDrawRowBorders(JPainter& p, const JRect& r,
								const JIndex firstRow, const JIndex lastRow,
								const bool drawBottomBorder);
	void	TableDrawColBorders(JPainter& p, const JRect& r,
								const JIndex firstCol, const JIndex lastCol,
								const bool drawRightBorder);

	void	TableSetBounds(const JCoordinate w, const JCoordinate h);
	void	TableAdjustBounds(const JCoordinate dw, const JCoordinate dh);

	static JInteger	GetCellSize(const JCoordinate& value);

	// not allowed

	JTable(const JTable&) = delete;
	JTable& operator=(const JTable&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kRowHeightChanged;
	static const JUtf8Byte* kAllRowHeightsChanged;
	static const JUtf8Byte* kRowsInserted;
	static const JUtf8Byte* kRowsRemoved;
	static const JUtf8Byte* kRowMoved;
	static const JUtf8Byte* kRowBorderWidthChanged;

	static const JUtf8Byte* kColWidthChanged;
	static const JUtf8Byte* kAllColWidthsChanged;
	static const JUtf8Byte* kColsInserted;
	static const JUtf8Byte* kColsRemoved;
	static const JUtf8Byte* kColMoved;
	static const JUtf8Byte* kColBorderWidthChanged;

	static const JUtf8Byte* kPrepareForTableDataMessage;	// message from JTableData
	static const JUtf8Byte* kPrepareForTableDataChange;	// about to switch to different JTableData
	static const JUtf8Byte* kTableDataChanged;				// now using different JTableData

protected:

	// base classes for JBroadcaster messages

	class RowColMessage : public JBroadcaster::Message
		{
		public:

			RowColMessage(const JUtf8Byte* type,
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

			bool
			Contains(const JIndex index) const
			{
				return GetFirstIndex() <= index && index <= GetLastIndex();
			};

		private:

			JIndex	itsFirstIndex;
			JSize	itsCount;
		};

	class RowColMoved : public JBroadcaster::Message
		{
		public:

			RowColMoved(const JUtf8Byte* type,
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

			BorderWidthChanged(const JUtf8Byte* type,
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

			bool	AdjustIndex(JIndex* index) const;

			bool
			AdjustCell(JPoint* cell) const
			{
				JIndex rowIndex   = cell->y;
				const bool ok = AdjustIndex(&rowIndex);
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

			bool	AdjustIndex(JIndex* index) const;

			bool
			AdjustCell(JPoint* cell) const
			{
				JIndex colIndex   = cell->x;
				const bool ok = AdjustIndex(&colIndex);
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

inline bool
JTable::RowIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= GetRowCount();
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
	JRunArrayIterator iter(*itsRowHeights, kJIteratorStartBefore, index);
	JCoordinate h;
	iter.Next(&h);
	return h;
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

inline bool
JTable::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return 1 <= index && index <= GetColCount();
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
	JRunArrayIterator iter(*itsColWidths, kJIteratorStartBefore, index);
	JCoordinate w = 0;
	iter.Next(&w);
	return w;
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

inline bool
JTable::CellValid
	(
	const JPoint& cell
	)
	const
{
	return RowIndexValid(cell.y) && ColIndexValid(cell.x);
}

/******************************************************************************
 Act like 1D list (protected)

 ******************************************************************************/

inline bool
JTable::WillActLike1DList()
	const
{
	return itsIs1DListFlag;
}

inline void
JTable::ShouldActLike1DList
	(
	const bool act1D
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
	JColorID*	color
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
	JColorID*	color
	)
	const
{
	*lineWidth = itsColBorderInfo.width;
	*color     = itsColBorderInfo.color;
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

inline bool
JTable::GetRowHeader
	(
	const JTable** table
	)
	const
{
	*table = itsRowHdrTable;
	return itsRowHdrTable != nullptr;
}

inline void
JTable::SetRowHeader
	(
	JTable* table
	)
{
	itsRowHdrTable = table;
}

inline bool
JTable::GetColHeader
	(
	const JTable** table
	)
	const
{
	*table = itsColHdrTable;
	return itsColHdrTable != nullptr;
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

inline JColorID
JTable::GetSelectionColor()
	const
{
	return itsSelectionColor;
}

inline void
JTable::SetSelectionColor
	(
	const JColorID color
	)
{
	itsSelectionColor = color;
	TableRefresh();
}

/******************************************************************************
 IsEditing

 ******************************************************************************/

inline bool
JTable::IsEditing()
	const
{
	return itsIsEditingFlag;
}

/******************************************************************************
 IsDraggingSelection

 ******************************************************************************/

inline bool
JTable::IsDraggingSelection()
	const
{
	return itsSelDragType != kInvalidDrag;
}

/******************************************************************************
 SetSelectionBehavior (protected)

 ******************************************************************************/

inline void
JTable::SetSelectionBehavior
	(
	const bool allowMultiple,
	const bool allowDiscont
	)
{
	itsAllowSelectMultipleFlag = allowMultiple;
	itsAllowSelectDiscontFlag  = allowDiscont;
}

#endif
