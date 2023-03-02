/******************************************************************************
 JTableSelection.h

	Interface for JTableSelection class

	Copyright (C) 1997 by John Lindal & Glenn Bach.

 ******************************************************************************/

#ifndef _H_JTableSelection
#define _H_JTableSelection

#include "JAuxTableData.h"
#include "JTableSelectionIterator.h"	// need definition of Direction

class JTableSelection : public JAuxTableData<bool>
{
public:

	JTableSelection(JTable* table);
	JTableSelection(JTable* table, const JTableSelection& source);

	~JTableSelection() override;

	void	SelectCell(const JPoint& cell, const bool on = true);
	void	SelectCell(const JIndex row, const JIndex col, 
					   const bool on = true);

	void	InvertCell(const JPoint& cell);
	void	InvertCell(const JIndex row, const JIndex col);

	bool	HasSelection() const;
	bool	IsSelected(const JPoint& cell) const;
	bool	IsSelected(const JIndex row, const JIndex col) const;
	JSize	GetSelectedCellCount() const;
	bool	GetSingleSelectedCell(JPoint* cell) const;
	bool	GetFirstSelectedCell(JPoint* cell,
				const JTableSelectionIterator::Direction d =
					JTableSelectionIterator::kIterateByCol) const;
	bool	GetLastSelectedCell(JPoint* cell,
				const JTableSelectionIterator::Direction d =
					JTableSelectionIterator::kIterateByCol) const;

	void	SelectRow(const JIndex rowIndex, const bool on = true);
	void	InvertRow(const JIndex rowIndex);

	void	SelectCol(const JIndex colIndex, const bool on = true);
	void	InvertCol(const JIndex rowIndex);

	void	SelectRect(const JRect& rect, const bool on = true);
	void	SelectRect(const JPoint& cell1, const JPoint& cell2,
					   const bool on = true);
	void	SelectRect(const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h,
					   const bool on = true);

	void	InvertRect(const JRect& rect);
	void	InvertRect(const JPoint& cell1, const JPoint& cell2);
	void	InvertRect(const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	void	SelectAll(const bool on = true);
	void	ClearSelection();

	// for selecting a rectangle of cells

	bool			HasBoat() const;
	const JPoint&	GetBoat() const;
	bool			GetBoat(JPoint* cell) const;
	void			SetBoat(const JPoint& cell);
	void			ClearBoat();

	bool			HasAnchor() const;
	const JPoint&	GetAnchor() const;
	bool			GetAnchor(JPoint* cell) const;
	void			SetAnchor(const JPoint& cell);
	void			ClearAnchor();

	bool	OKToExtendSelection() const;
	bool	ExtendSelection(const JPoint& newBoat);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// stored mainly for convenience, but also updated when the table changes

	JPoint	itsBoat;
	JPoint	itsAnchor;
	bool	itsReselectAfterChangeFlag;

private:

	bool	UndoSelection();

	void	AdjustIndexAfterRemove(const JIndex origBoat, const JIndex origAnchor,
								   const JIndex firstIndex, const JIndex maxIndex,
								   JCoordinate* boat, JCoordinate* anchor) const;

	static bool	InvertSelection(const bool&);
};


/******************************************************************************
 SelectCell

 ******************************************************************************/

inline void
JTableSelection::SelectCell
	(
	const JPoint&	cell,
	const bool	on
	)
{
	SetElement(cell.y, cell.x, on);
}

inline void
JTableSelection::SelectCell
	(
	const JIndex	row,
	const JIndex	col,
	const bool	on
	)
{
	SetElement(row,col, on);
}

/******************************************************************************
 InvertCell

 ******************************************************************************/

inline void
JTableSelection::InvertCell
	(
	const JPoint& cell
	)
{
	SetElement(cell.y, cell.x, !GetElement(cell.y, cell.x));
}

inline void
JTableSelection::InvertCell
	(
	const JIndex row,
	const JIndex col
	)
{
	SetElement(row,col, !GetElement(row, col));
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

inline bool
JTableSelection::HasSelection()
	const
{
	bool value;
	return !IsEmpty() && !(AllElementsEqual(&value) && !value);
}

/******************************************************************************
 IsSelected

 ******************************************************************************/

inline bool
JTableSelection::IsSelected
	(
	const JPoint& cell
	)
	const
{
	return GetElement(cell);
}

inline bool
JTableSelection::IsSelected
	(
	const JIndex row,
	const JIndex col
	)
	const
{
	return GetElement(row,col);
}

/******************************************************************************
 GetSelectedCellCount

 ******************************************************************************/

inline JSize
JTableSelection::GetSelectedCellCount()
	const
{
	return CountElementsWithValue(true);
}

/******************************************************************************
 SelectRow

 ******************************************************************************/

inline void
JTableSelection::SelectRow
	(
	const JIndex	rowIndex,
	const bool	on
	)
{
	SetRow(rowIndex, on);
}

/******************************************************************************
 InvertRow

 ******************************************************************************/

inline void
JTableSelection::InvertRow
	(
	const JIndex rowIndex
	)
{
	ApplyToRect(JRect(rowIndex, 1, rowIndex+1, GetColCount()+1), InvertSelection);
}

/******************************************************************************
 SelectCol

 ******************************************************************************/

inline void
JTableSelection::SelectCol
	(
	const JIndex	colIndex,
	const bool	on
	)
{
	SetCol(colIndex, on);
}

/******************************************************************************
 InvertCol

 ******************************************************************************/

inline void
JTableSelection::InvertCol
	(
	const JIndex colIndex
	)
{
	ApplyToRect(JRect(1, colIndex, GetRowCount()+1, colIndex+1), InvertSelection);
}

/******************************************************************************
 SelectRect

 ******************************************************************************/

inline void
JTableSelection::SelectRect
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const bool		on
	)
{
	SetRect(x,y,w,h, on);
}

inline void
JTableSelection::SelectRect
	(
	const JRect&	rect,
	const bool	on
	)
{
	SetRect(rect, on);
}

inline void
JTableSelection::SelectRect
	(
	const JPoint&	cell1,
	const JPoint&	cell2,
	const bool	on
	)
{
	SelectRect(JRect(cell1, cell2), on);
}

/******************************************************************************
 InvertRect

 ******************************************************************************/

inline void
JTableSelection::InvertRect
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	ApplyToRect(x,y,w,h, InvertSelection);
}

inline void
JTableSelection::InvertRect
	(
	const JRect& rect
	)
{
	ApplyToRect(rect, InvertSelection);
}

inline void
JTableSelection::InvertRect
	(
	const JPoint& cell1,
	const JPoint& cell2
	)
{
	InvertRect(JRect(cell1, cell2));
}

/******************************************************************************
 Selection endpoints

	This is provided as a convenience since most tables need to store it.

 ******************************************************************************/

inline bool
JTableSelection::HasBoat()
	const
{
	return CellValid(itsBoat);
}

inline const JPoint&
JTableSelection::GetBoat()
	const
{
	return itsBoat;
}

inline bool
JTableSelection::GetBoat
	(
	JPoint* cell
	)
	const
{
	*cell = itsBoat;
	return CellValid(itsBoat);
}

inline void
JTableSelection::SetBoat
	(
	const JPoint& cell
	)
{
	itsBoat = cell;
}

inline void
JTableSelection::ClearBoat()
{
	itsBoat.x = itsBoat.y = 0;
}

inline bool
JTableSelection::HasAnchor()
	const
{
	return CellValid(itsAnchor);
}

inline const JPoint&
JTableSelection::GetAnchor()
	const
{
	return itsAnchor;
}

inline bool
JTableSelection::GetAnchor
	(
	JPoint* cell
	)
	const
{
	*cell = itsAnchor;
	return CellValid(itsAnchor);
}

inline void
JTableSelection::SetAnchor
	(
	const JPoint& cell
	)
{
	itsAnchor = cell;
}

inline void
JTableSelection::ClearAnchor()
{
	itsAnchor.x = itsAnchor.y = 0;
}

/******************************************************************************
 OKToExtendSelection

 ******************************************************************************/

inline bool
JTableSelection::OKToExtendSelection()
	const
{
	return CellValid(itsBoat) && CellValid(itsAnchor);
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

inline void
JTableSelection::SelectAll
	(
	const bool on
	)
{
	ClearBoat();
	ClearAnchor();
	SetAllElements(on);
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

inline void
JTableSelection::ClearSelection()
{
	SelectAll(false);
}

#endif
