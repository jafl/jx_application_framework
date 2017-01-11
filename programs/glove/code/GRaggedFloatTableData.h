/******************************************************************************
 GRaggedFloatTableData.h

	Interface for GRaggedFloatTableData class

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GRaggedFloatTableData
#define _H_GRaggedFloatTableData

#include <JTableData.h>
#include <JPtrArray.h>

class GRaggedFloatTableData : public JTableData
{
public:

	GRaggedFloatTableData(const JFloat& defValue);
	GRaggedFloatTableData(const GRaggedFloatTableData& source);

	virtual ~GRaggedFloatTableData();

	JBoolean	GetElement(const JIndex row, const JIndex col, JFloat* value) const;
	JBoolean	GetElement(const JPoint& cell, JFloat* value) const;
	void		SetElement(const JIndex row, const JIndex col, const JFloat data);
	void		SetElement(const JPoint& cell, const JFloat data);

	void	GetRow(const JIndex index, JArray<JFloat>* rowData) const;
	void	SetRow(const JIndex index, const JArray<JFloat>& rowData);

	void	GetCol(const JIndex index, JOrderedSet<JFloat>* colData) const;
	void	SetCol(const JIndex index, const JOrderedSet<JFloat>& colData);

	const JArray<JFloat>&	GetColPointer(const JIndex index) const;

	void	InsertRow(const JIndex index, const JOrderedSet<JFloat>* initData = NULL);
	void	InsertRows(const JIndex index, const JSize count, const JOrderedSet<JFloat>* initData = NULL);
	void	PrependRow();
	void	DuplicateRow(const JIndex index);
	void	RemoveRow(const JIndex index);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCol(const JIndex index, const JOrderedSet<JFloat>* initData = NULL);
	void	InsertCols(const JIndex index, const JSize count, const JOrderedSet<JFloat>* initData = NULL);
	void	PrependCol(const JOrderedSet<JFloat>* initData = NULL);
	void	AppendCol(const JOrderedSet<JFloat>* initData = NULL);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	void	InsertElement(const JIndex row, const JIndex col, const JFloat value = 0);
	void	InsertElement(const JPoint& cell, const JFloat value = 0);
	void	PrependElement(const JIndex col, const JFloat value = 0);
	void	AppendElement(const JIndex col, const JFloat value = 0);
	void	DuplicateElement(const JIndex row, const JIndex col);
	void	DuplicateElement(const JPoint& cell);
	void	RemoveElement(const JIndex row, const JIndex col);
	void	RemoveElement(const JPoint& cell);
	void	RemoveAllElements(const JIndex col);
	void	MoveElement(const JIndex origRow, const JIndex origCol,
						const JIndex newRow, const JIndex newCol);
	void	MoveElement(const JPoint& origCell, const JPoint& newCell);

	JSize		GetDataColCount() const;
	JBoolean	ColIndexValid(const JIndex index) const;
	JSize		GetDataRowCount(const JIndex index) const;
	JBoolean	CellValid(const JIndex rowIndex, const JIndex colIndex) const;
	JBoolean	CellValid(const JPoint& cell) const;
	JBoolean	FindColumn(const JArray<JFloat>* array, JIndex *index) const;

	void	ShouldBroadcast(const JBoolean on);

private:

	JPtrArray< JArray<JFloat> >*	itsCols;
	const JFloat					itsDefValue;
	JBoolean						itsBroadcast;

private:

	void 	CreateCellIfNeeded(const JIndex row, const JIndex col);
	void 	CreateCellIfNeeded(const JPoint cell);
	JSize 	GetMaxRowCount();

	// not allowed

	const GRaggedFloatTableData&	operator=(const GRaggedFloatTableData& source);

public:

	static const JCharacter* kElementInserted;
	static const JCharacter* kElementRemoved;
	static const JCharacter* kDataChanged;

	class ElementInserted : public JBroadcaster::Message
		{
		public:

			ElementInserted(const JIndex row, const JIndex col)
				:
				JBroadcaster::Message(kElementInserted),
				itsRow(row),
				itsCol(col)
				{ };

			JIndex
			GetRow() const
			{
				return itsRow;
			};

			JIndex
			GetCol() const
			{
				return itsCol;
			};

		private:

			JIndex itsRow;
			JIndex itsCol;
		};

	class ElementRemoved : public JBroadcaster::Message
		{
		public:

			ElementRemoved(const JIndex row, const JIndex col)
				:
				JBroadcaster::Message(kElementRemoved),
				itsRow(row),
				itsCol(col)
				{ };

			JIndex
			GetRow() const
			{
				return itsRow;
			};

			JIndex
			GetCol() const
			{
				return itsCol;
			};

		private:

			JIndex itsRow;
			JIndex itsCol;
		};

	class DataChanged : public JBroadcaster::Message
		{
		public:

			DataChanged()
				:
				JBroadcaster::Message(kDataChanged)
				{ };
		};
};


/******************************************************************************
 PrependRow

 ******************************************************************************/

inline void
GRaggedFloatTableData::PrependRow()
{
	InsertRow(1);
}

/******************************************************************************
 Column manipulations

 ******************************************************************************/

inline void
GRaggedFloatTableData::PrependCol
	(
	const JOrderedSet<JFloat>* initData
	)
{
	InsertCol(1, initData);
}

inline void
GRaggedFloatTableData::AppendCol
	(
	const JOrderedSet<JFloat>* initData
	)
{
	InsertCol(itsCols->GetElementCount()+1, initData);
}

/******************************************************************************
 GetColPointer

 ******************************************************************************/

inline const JArray<JFloat>&
GRaggedFloatTableData::GetColPointer
	(
	const JIndex index
	)
	const
{
	return *(itsCols->NthElement(index));
}

/******************************************************************************
 PrependElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::PrependElement
	(
	const JIndex col,
	const JFloat value
	)
{
	InsertElement(1, col, value);
}

/******************************************************************************
 AppendElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::AppendElement
	(
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->NthElement(col);
	InsertElement(dataCol->GetElementCount()+1, col, value);
}

/******************************************************************************
 InsertElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::InsertElement
	(
	const JPoint&	cell,
	const JFloat	value
	)
{
	InsertElement(cell.y, cell.x, value);
}

/******************************************************************************
 RemoveElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::RemoveElement
	(
	const JPoint& cell
	)
{
	RemoveElement(cell.y, cell.x);
}

/******************************************************************************
 MoveElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::MoveElement
	(
	const JPoint& origCell,
	const JPoint& newCell
	)
{
	MoveElement(origCell.y, origCell.x, newCell.y, newCell.x);
}

/******************************************************************************
 DuplicateElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::DuplicateElement
	(
	const JPoint& cell
	)
{
	DuplicateElement(cell.y, cell.x);
}

/******************************************************************************
 GetElement

 ******************************************************************************/

inline JBoolean
GRaggedFloatTableData::GetElement
	(
	const JPoint&	cell,
	JFloat*			value
	)
	const
{
	return GetElement(cell.y, cell.x, value);
}

/******************************************************************************
 SetElement

 ******************************************************************************/

inline void
GRaggedFloatTableData::SetElement
	(
	const JPoint&	cell,
	const JFloat	data
	)
{
	SetElement(cell.y, cell.x, data);
}

/******************************************************************************
 ShouldBroadcast

 ******************************************************************************/

inline void
GRaggedFloatTableData::ShouldBroadcast
	(
	const JBoolean on
	)
{
	const JBoolean wasOn = itsBroadcast;
	itsBroadcast = on;
	if (!wasOn && itsBroadcast)
		{
		Broadcast(DataChanged());
		}
}

/******************************************************************************
 GetDataColCount

 ******************************************************************************/

inline JSize
GRaggedFloatTableData::GetDataColCount()
	const
{
	return itsCols->GetElementCount();
}

/******************************************************************************
 ColIndexValid

 ******************************************************************************/

inline JBoolean
GRaggedFloatTableData::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return itsCols->IndexValid(index);
}

/******************************************************************************
 GetDataRowCount

 ******************************************************************************/

inline JSize
GRaggedFloatTableData::GetDataRowCount
	(
	const JIndex index
	)
	const
{
	const JArray<JFloat>* dataCol = itsCols->NthElement(index);
	return dataCol->GetElementCount();
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline JBoolean
GRaggedFloatTableData::CellValid
	(
	const JIndex rowIndex,
	const JIndex colIndex
	)
	const
{
	return JI2B(itsCols->IndexValid(colIndex) &&
				(itsCols->NthElement(colIndex))->IndexValid(rowIndex));
}

inline JBoolean
GRaggedFloatTableData::CellValid
	(
	const JPoint& cell
	)
	const
{
	return CellValid(cell.y, cell.x);
}

/******************************************************************************
 FindColumn

 ******************************************************************************/

inline JBoolean
GRaggedFloatTableData::FindColumn
	(
	const JArray<JFloat>*	array,
	JIndex*					index
	)
	const
{
	return itsCols->Find(array, index);
}

#endif
