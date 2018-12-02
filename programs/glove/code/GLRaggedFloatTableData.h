/******************************************************************************
 GLRaggedFloatTableData.h

	Interface for GLRaggedFloatTableData class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GRaggedFloatTableData
#define _H_GRaggedFloatTableData

#include <JTableData.h>
#include <JPtrArray.h>

class GLRaggedFloatTableData : public JTableData
{
public:

	GLRaggedFloatTableData(const JFloat& defValue);
	GLRaggedFloatTableData(const GLRaggedFloatTableData& source);

	virtual ~GLRaggedFloatTableData();

	JBoolean	GetElement(const JIndex row, const JIndex col, JFloat* value) const;
	JBoolean	GetElement(const JPoint& cell, JFloat* value) const;
	void		SetElement(const JIndex row, const JIndex col, const JFloat data);
	void		SetElement(const JPoint& cell, const JFloat data);

	void	GetRow(const JIndex index, JArray<JFloat>* rowData) const;
	void	SetRow(const JIndex index, const JArray<JFloat>& rowData);

	void	GetCol(const JIndex index, JList<JFloat>* colData) const;
	void	SetCol(const JIndex index, const JList<JFloat>& colData);

	const JArray<JFloat>&	GetColPointer(const JIndex index) const;

	void	InsertRow(const JIndex index, const JList<JFloat>* initData = nullptr);
	void	InsertRows(const JIndex index, const JSize count, const JList<JFloat>* initData = nullptr);
	void	PrependRow();
	void	DuplicateRow(const JIndex index);
	void	RemoveRow(const JIndex index);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCol(const JIndex index, const JList<JFloat>* initData = nullptr);
	void	InsertCols(const JIndex index, const JSize count, const JList<JFloat>* initData = nullptr);
	void	PrependCol(const JList<JFloat>* initData = nullptr);
	void	AppendCol(const JList<JFloat>* initData = nullptr);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	void	InsertElement(const JIndex row, const JIndex col, const JFloat value);
	void	InsertElement(const JPoint& cell, const JFloat value);
	void	PrependElement(const JIndex col, const JFloat value);
	void	AppendElement(const JIndex col, const JFloat value);
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

	const GLRaggedFloatTableData&	operator=(const GLRaggedFloatTableData& source);

public:

	static const JUtf8Byte* kElementInserted;
	static const JUtf8Byte* kElementRemoved;
	static const JUtf8Byte* kDataChanged;

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
GLRaggedFloatTableData::PrependRow()
{
	InsertRow(1);
}

/******************************************************************************
 Column manipulations

 ******************************************************************************/

inline void
GLRaggedFloatTableData::PrependCol
	(
	const JList<JFloat>* initData
	)
{
	InsertCol(1, initData);
}

inline void
GLRaggedFloatTableData::AppendCol
	(
	const JList<JFloat>* initData
	)
{
	InsertCol(itsCols->GetElementCount()+1, initData);
}

/******************************************************************************
 GetColPointer

 ******************************************************************************/

inline const JArray<JFloat>&
GLRaggedFloatTableData::GetColPointer
	(
	const JIndex index
	)
	const
{
	return *(itsCols->GetElement(index));
}

/******************************************************************************
 PrependElement

 ******************************************************************************/

inline void
GLRaggedFloatTableData::PrependElement
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
GLRaggedFloatTableData::AppendElement
	(
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->GetElement(col);
	InsertElement(dataCol->GetElementCount()+1, col, value);
}

/******************************************************************************
 InsertElement

 ******************************************************************************/

inline void
GLRaggedFloatTableData::InsertElement
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
GLRaggedFloatTableData::RemoveElement
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
GLRaggedFloatTableData::MoveElement
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
GLRaggedFloatTableData::DuplicateElement
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
GLRaggedFloatTableData::GetElement
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
GLRaggedFloatTableData::SetElement
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
GLRaggedFloatTableData::ShouldBroadcast
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
GLRaggedFloatTableData::GetDataColCount()
	const
{
	return itsCols->GetElementCount();
}

/******************************************************************************
 ColIndexValid

 ******************************************************************************/

inline JBoolean
GLRaggedFloatTableData::ColIndexValid
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
GLRaggedFloatTableData::GetDataRowCount
	(
	const JIndex index
	)
	const
{
	const JArray<JFloat>* dataCol = itsCols->GetElement(index);
	return dataCol->GetElementCount();
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline JBoolean
GLRaggedFloatTableData::CellValid
	(
	const JIndex rowIndex,
	const JIndex colIndex
	)
	const
{
	return JI2B(itsCols->IndexValid(colIndex) &&
				(itsCols->GetElement(colIndex))->IndexValid(rowIndex));
}

inline JBoolean
GLRaggedFloatTableData::CellValid
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
GLRaggedFloatTableData::FindColumn
	(
	const JArray<JFloat>*	array,
	JIndex*					index
	)
	const
{
	return itsCols->Find(array, index);
}

#endif
