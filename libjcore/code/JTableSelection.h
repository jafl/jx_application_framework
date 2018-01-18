/******************************************************************************
 JTableSelection.h

	Interface for JTableSelection class

	Copyright (C) 1997 by John Lindal & Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTableSelection
#define _H_JTableSelection

#include <JAuxTableData.h>
#include <JTableSelectionIterator.h>	// need definition of Direction

class JTableSelection : public JAuxTableData<JBoolean>
{
public:

	JTableSelection(JTable* table);
	JTableSelection(JTable* table, const JTableSelection& source);

	virtual ~JTableSelection();

	void	SelectCell(const JPoint& cell, const JBoolean on = kJTrue);
	void	SelectCell(const JIndex row, const JIndex col, 
					   const JBoolean on = kJTrue);

	void	InvertCell(const JPoint& cell);
	void	InvertCell(const JIndex row, const JIndex col);

	JBoolean	HasSelection() const;
	JBoolean	IsSelected(const JPoint& cell) const;
	JBoolean	IsSelected(const JIndex row, const JIndex col) const;
	JSize		GetSelectedCellCount() const;
	JBoolean	GetSingleSelectedCell(JPoint* cell) const;
	JBoolean	GetFirstSelectedCell(JPoint* cell,
					const JTableSelectionIterator::Direction d =
						JTableSelectionIterator::kIterateByCol) const;
	JBoolean	GetLastSelectedCell(JPoint* cell,
					const JTableSelectionIterator::Direction d =
						JTableSelectionIterator::kIterateByCol) const;

	void	SelectRow(const JIndex rowIndex, const JBoolean on = kJTrue);
	void	InvertRow(const JIndex rowIndex);

	void	SelectCol(const JIndex colIndex, const JBoolean on = kJTrue);
	void	InvertCol(const JIndex rowIndex);

	void	SelectRect(const JRect& rect, const JBoolean on = kJTrue);
	void	SelectRect(const JPoint& cell1, const JPoint& cell2,
					   const JBoolean on = kJTrue);
	void	SelectRect(const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h,
					   const JBoolean on = kJTrue);

	void	InvertRect(const JRect& rect);
	void	InvertRect(const JPoint& cell1, const JPoint& cell2);
	void	InvertRect(const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	void	SelectAll(const JBoolean on = kJTrue);
	void	ClearSelection();

	// for selecting a rectangle of cells

	JBoolean		HasBoat() const;
	const JPoint&	GetBoat() const;
	JBoolean		GetBoat(JPoint* cell) const;
	void			SetBoat(const JPoint& cell);
	void			ClearBoat();

	JBoolean		HasAnchor() const;
	const JPoint&	GetAnchor() const;
	JBoolean		GetAnchor(JPoint* cell) const;
	void			SetAnchor(const JPoint& cell);
	void			ClearAnchor();

	JBoolean	OKToExtendSelection() const;
	JBoolean	ExtendSelection(const JPoint& newBoat);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// stored mainly for convenience, but also updated when the table changes

	JPoint		itsBoat;
	JPoint		itsAnchor;
	JBoolean	itsReselectAfterChangeFlag;

private:

	JBoolean	UndoSelection();

	void	AdjustIndexAfterRemove(const JIndex origBoat, const JIndex origAnchor,
								   const JIndex firstIndex, const JIndex maxIndex,
								   JCoordinate* boat, JCoordinate* anchor) const;

	static JBoolean	InvertSelection(const JBoolean&);

	// not allowed

	JTableSelection(const JTableSelection& source);
	const JTableSelection& operator=(const JTableSelection& source);
};


/******************************************************************************
 SelectCell

 ******************************************************************************/

inline void
JTableSelection::SelectCell
	(
	const JPoint&	cell,
	const JBoolean	on
	)
{
	SetElement(cell.y, cell.x, on);
}

inline void
JTableSelection::SelectCell
	(
	const JIndex	row,
	const JIndex	col,
	const JBoolean	on
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

inline JBoolean
JTableSelection::HasSelection()
	const
{
	JBoolean value;
	return JConvertToBoolean(
			!IsEmpty() && !(AllElementsEqual(&value) && !value) );
}

/******************************************************************************
 IsSelected

 ******************************************************************************/

inline JBoolean
JTableSelection::IsSelected
	(
	const JPoint& cell
	)
	const
{
	return GetElement(cell);
}

inline JBoolean
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
	return CountElementsWithValue(kJTrue);
}

/******************************************************************************
 SelectRow

 ******************************************************************************/

inline void
JTableSelection::SelectRow
	(
	const JIndex	rowIndex,
	const JBoolean	on
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
	const JBoolean	on
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
	const JBoolean		on
	)
{
	SetRect(x,y,w,h, on);
}

inline void
JTableSelection::SelectRect
	(
	const JRect&	rect,
	const JBoolean	on
	)
{
	SetRect(rect, on);
}

inline void
JTableSelection::SelectRect
	(
	const JPoint&	cell1,
	const JPoint&	cell2,
	const JBoolean	on
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

inline JBoolean
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

inline JBoolean
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

inline JBoolean
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

inline JBoolean
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

inline JBoolean
JTableSelection::OKToExtendSelection()
	const
{
	return JI2B( CellValid(itsBoat) && CellValid(itsAnchor) );
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

inline void
JTableSelection::SelectAll
	(
	const JBoolean on
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
	SelectAll(kJFalse);
}

#endif
