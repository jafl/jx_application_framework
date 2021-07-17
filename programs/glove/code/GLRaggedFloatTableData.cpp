/******************************************************************************
 GLRaggedFloatTableData.cpp

	Class for storing floats in a table of ragged columns.

	BASE CLASS = JTableData

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLRaggedFloatTableData.h"
#include <jAssert.h>

const JUtf8Byte* GLRaggedFloatTableData::kElementInserted = "ElementInserted::GLRaggedFloatTableData";
const JUtf8Byte* GLRaggedFloatTableData::kElementRemoved  = "ElementRemoved::GLRaggedFloatTableData";
const JUtf8Byte* GLRaggedFloatTableData::kDataChanged     = "DataChanged::GLRaggedFloatTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

GLRaggedFloatTableData::GLRaggedFloatTableData
	(
	const JFloat& defValue
	)
	:
	JTableData(),
	itsDefValue(defValue)
{
	itsCols = jnew JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != nullptr );

	const JSize kInitialColCount = 20;
	const JSize kInitialRowCount = 100;
	ColsAdded(kInitialColCount);
	Broadcast(JTableData::ColsInserted(1, kInitialColCount));

	RowsAdded(kInitialRowCount);
	Broadcast(JTableData::RowsInserted(1, kInitialRowCount));

	itsBroadcast = true;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

GLRaggedFloatTableData::GLRaggedFloatTableData
	(
	const GLRaggedFloatTableData& source
	)
	:
	JTableData(source),
	itsDefValue(source.itsDefValue),
	itsBroadcast(source.itsBroadcast)
{
	itsCols = jnew JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != nullptr );

	const JSize count = (source.itsCols)->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JArray<JFloat>* origColData = (source.itsCols)->GetElement(i);
		JArray<JFloat>* newColData = jnew JArray<JFloat>(*origColData);
		assert( newColData != nullptr );
		itsCols->Append(newColData);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLRaggedFloatTableData::~GLRaggedFloatTableData()
{
	itsCols->DeleteAll();
	jdelete itsCols;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

bool
GLRaggedFloatTableData::GetElement
	(
	const JIndex row,
	const JIndex col,
	JFloat* value
	)
	const
{
	if (CellValid(row,col) == false)
		{
		return false;
		}

	const JArray<JFloat>* dataCol = itsCols->GetElement(col);
	*value =  dataCol->GetElement(row);
	return true;
}

/******************************************************************************
 CreateCellIfNeeded (private)

 ******************************************************************************/

inline void
GLRaggedFloatTableData::CreateCellIfNeeded
	(
	const JPoint cell
	)
{
	CreateCellIfNeeded(cell.y, cell.x);
}

/******************************************************************************
 CreateCellIfNeeded

 ******************************************************************************/

void
GLRaggedFloatTableData::CreateCellIfNeeded
	(
	const JIndex row,
	const JIndex col
	)
{
	while (col > itsCols->GetElementCount())
		{
		AppendCol();
		}

	const bool origBroadcast = itsBroadcast;
	if (origBroadcast)
		{
		ShouldBroadcast(false);
		}

	const JArray<JFloat>* dataCol = itsCols->GetElement(col);
	while (row > dataCol->GetElementCount())
		{
		AppendElement(col, itsDefValue);
		}

	if (origBroadcast)
		{
		ShouldBroadcast(true);
		}
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
GLRaggedFloatTableData::SetElement
	(
	const JIndex	row,
	const JIndex	col,
	const JFloat	data
	)
{
	CreateCellIfNeeded(row, col);
	JArray<JFloat>* dataCol = itsCols->GetElement(col);
	dataCol->SetElement(row, data);
	if (itsBroadcast)
		{
		Broadcast(JTableData::RectChanged(row,col));
		}
}

/******************************************************************************
 GetRow

 ******************************************************************************/

void
GLRaggedFloatTableData::GetRow
	(
	const JIndex			index,
	JArray<JFloat>*	rowData
	)
	const
{
	rowData->RemoveAll();

	const JSize colCount = GetDataColCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		const JArray<JFloat>* dataCol = itsCols->GetElement(i);
		rowData->AppendElement(dataCol->GetElement(index));
		}
}

/******************************************************************************
 SetRow

 ******************************************************************************/

void
GLRaggedFloatTableData::SetRow
	(
	const JIndex				index,
	const JArray<JFloat>&	rowData
	)
{
	const JSize colCount = GetDataColCount();
	assert( rowData.GetElementCount() == colCount );

	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* dataCol = itsCols->GetElement(i);
		dataCol->SetElement(index, rowData.GetElement(i));
		}

	Broadcast(JTableData::RectChanged(JRect(index, 1, index+1, colCount+1)));
}

/******************************************************************************
 GetCol

	operator= doesn't work because it could be -any- class derived from JList.

 ******************************************************************************/

void
GLRaggedFloatTableData::GetCol
	(
	const JIndex	index,
	JList<JFloat>*	colData
	)
	const
{
	colData->RemoveAll();

	const JArray<JFloat>* dataCol = itsCols->GetElement(index);
	const JSize rowCount = dataCol->GetElementCount();

	for (JIndex i=1; i<=rowCount; i++)
		{
		colData->AppendElement(dataCol->GetElement(i));
		}
}

/******************************************************************************
 SetCol

 ******************************************************************************/

void
GLRaggedFloatTableData::SetCol
	(
	const JIndex			index,
	const JList<JFloat>&	colData
	)
{
	JArray<JFloat>* dataCol = itsCols->GetElement(index);
	const JSize rowCount = colData.GetElementCount();
	CreateCellIfNeeded(rowCount, index);

	JListIterator<JFloat>* iter = colData.NewIterator();
	JFloat v;
	JIndex i = 1;
	while (iter->Next(&v))
		{
		dataCol->SetElement(i, v);
		i++;
		}

	jdelete iter;
	iter = nullptr;

	JRect rect(1, index, rowCount + 1, index + 1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::RectChanged(rect));
		}
}

/******************************************************************************
 InsertRow

 ******************************************************************************/

void
GLRaggedFloatTableData::InsertRow
	(
	const JIndex			index,
	const JList<JFloat>*	initData
	)
{
	const JSize colCount = itsCols->GetElementCount();
	assert( initData == nullptr || initData->GetElementCount() == colCount );

	JListIterator<JFloat>* iter = nullptr;
	if (initData != nullptr)
		{
		iter = initData->NewIterator();
		}

	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->GetElement(i);
		if (index <= colData->GetElementCount())
			{
			JFloat value = 0;
			if (iter != nullptr)
				{
				iter->Next(&value);
				}
			InsertElement(index, i, value);
			}
		}

	jdelete iter;
}

/******************************************************************************
 InsertRows (public)

 ******************************************************************************/

void
GLRaggedFloatTableData::InsertRows
	(
	const JIndex				index,
	const JSize 				count,
	const JList<JFloat>*	initData
	)
{
	for (JIndex i = 0; i < count; i++)
		{
		InsertRow(i + index, initData);
		}
}

/******************************************************************************
 DuplicateRow

 ******************************************************************************/

void
GLRaggedFloatTableData::DuplicateRow
	(
	const JIndex index
	)
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->GetElement(i);
		const JSize rowCount = colData->GetElementCount();

		if (index <= rowCount)
			{
			const JFloat element = colData->GetElement(index);
			colData->InsertElementAtIndex(index, element);
			}
		}

	RowsAdded(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::RowDuplicated(index, index));
		}
}

/******************************************************************************
 RemoveRow

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveRow
	(
	const JIndex index
	)
{
	assert( index < GetRowCount() );

	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->GetElement(i);
		const JSize rowCount = colData->GetElementCount();

		if (index <= rowCount)
			{
			RemoveElement(index, i);
			}
		}

}

/******************************************************************************
 RemoveAllRows

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveAllRows()
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->GetElement(i);
		colData->RemoveAll();
		}
	const JSize count	= GetRowCount();
	SetRowCount(0);
	if (itsBroadcast)
		{
		Broadcast(JTableData::RowsRemoved(1, count));
		}
	RowsAdded(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::RowsInserted(1, 1));
		}
}

/******************************************************************************
 MoveRow

 ******************************************************************************/

void
GLRaggedFloatTableData::MoveRow
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->GetElement(i);
		const JSize rowCount = colData->GetElementCount();
		if ( (origIndex <= rowCount) && (newIndex <= rowCount) )
			{
			colData->MoveElementToIndex(origIndex, newIndex);
			}
		}

	if (itsBroadcast)
		{
		Broadcast(JTableData::RowMoved(origIndex, newIndex));
		}
}

/******************************************************************************
 InsertCol

 ******************************************************************************/

void
GLRaggedFloatTableData::InsertCol
	(
	const JIndex			index,
	const JList<JFloat>*	initData
	)
{
	JIndex trueIndex = index;
	const JIndex maxIndex = itsCols->GetElementCount()+1;
	if (trueIndex > maxIndex)
		{
		trueIndex = maxIndex;
		}

	JArray<JFloat>* colData = jnew JArray<JFloat>;
	assert( colData != nullptr );
	itsCols->InsertAtIndex(trueIndex, colData);

	if (initData != nullptr)
		{
		JListIterator<JFloat>* iter = initData->NewIterator();

		JFloat v;
		JIndex i = 1;
		while (iter->Next(&v))
			{
			colData->InsertElementAtIndex(i, v);
			}

		jdelete iter;
		}

	ColsAdded(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColsInserted(trueIndex, 1));
		}
}

/******************************************************************************
 InsertCols

 ******************************************************************************/

void
GLRaggedFloatTableData::InsertCols
	(
	const JIndex				index,
	const JSize 				count,
	const JList<JFloat>*	initData
	)
{
	for (JIndex i = 0; i < count; i++)
		{
		InsertCol(i + index, initData);
		}
}

/******************************************************************************
 DuplicateCol

 ******************************************************************************/

void
GLRaggedFloatTableData::DuplicateCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	JIndex trueIndex = newIndex;
	const JIndex maxIndex = itsCols->GetElementCount()+1;
	if (trueIndex > maxIndex)
		{
		trueIndex = maxIndex;
		}

	JArray<JFloat>* origColData = itsCols->GetElement(origIndex);
	JArray<JFloat>* newColData = jnew JArray<JFloat>(*origColData);
	assert( newColData != nullptr );
	itsCols->InsertAtIndex(trueIndex, newColData);

	ColsAdded(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColDuplicated(origIndex, trueIndex));
		}
}

/******************************************************************************
 RemoveCol

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveCol
	(
	const JIndex index
	)
{
	assert( ColIndexValid(index) );
	itsCols->DeleteElement(index);
/*	if (itsCols->GetElementCount() != 1)
		{
		itsCols->DeleteElement(index);
		}
	else
		{
		RemoveAllElements(index);
		itsCols->DeleteElement(index);
		}
*/
	ColsDeleted(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColsRemoved(index, 1));
		}
}

/******************************************************************************
 RemoveAllCols

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveAllCols()
{
	itsCols->DeleteAll();

	const JSize count	= GetColCount();
	SetColCount(0);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColsRemoved(1, count));
		}
	ColsAdded(1);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColsInserted(1, 1));
		}
}

/******************************************************************************
 MoveCol

 ******************************************************************************/

void
GLRaggedFloatTableData::MoveCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	itsCols->MoveElementToIndex(origIndex, newIndex);
	if (itsBroadcast)
		{
		Broadcast(JTableData::ColMoved(origIndex, newIndex));
		}
}

/******************************************************************************
 InsertElement

 ******************************************************************************/

void
GLRaggedFloatTableData::InsertElement
	(
	const JIndex row,
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->GetElement(col);
	dataCol->InsertElementAtIndex(row, value);
	const JSize rowCount = dataCol->GetElementCount();
	if (itsBroadcast)
		{
		Broadcast(GLRaggedFloatTableData::ElementInserted(row, col));
		}

	if (rowCount == GetRowCount())
		{
		RowsAdded(1);
		if (itsBroadcast)
			{
			Broadcast(JTableData::RowsInserted(rowCount+1, 1));
			}
		}
}

/******************************************************************************
 DuplicateElement

 ******************************************************************************/

void
GLRaggedFloatTableData::DuplicateElement
	(
	const JIndex row,
	const JIndex col
	)
{
	JFloat value;
	if (GetElement(row,col, &value))
		{
		InsertElement(row, col, value);
		}
}

/******************************************************************************
 RemoveElement

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveElement
	(
	const JIndex row,
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->GetElement(col);
	const JSize rowCount = dataCol->GetElementCount();
	if (row <= rowCount)
		{
		dataCol->RemoveElement(row);
		}
	if (itsBroadcast)
		{
		Broadcast(GLRaggedFloatTableData::ElementRemoved(row, col));
		}

	if (GetMaxRowCount() == GetRowCount() - 2)
		{
		if (itsBroadcast)
			{
			Broadcast(JTableData::RowsRemoved(GetRowCount(), 1));
			}
		RowsDeleted(1);
		}
}

/******************************************************************************
 RemoveAllElements

 ******************************************************************************/

void
GLRaggedFloatTableData::RemoveAllElements
	(
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->GetElement(col);
	const JSize rowCount = dataCol->GetElementCount();
	for (JSize i = 1; i <= rowCount; i++)
		{
		RemoveElement(1, col);
		}
}

/******************************************************************************
 MoveElement

 ******************************************************************************/

void
GLRaggedFloatTableData::MoveElement
	(
	const JIndex origRow,
	const JIndex origCol,
	const JIndex newRow,
	const JIndex newCol
	)
{
	JFloat value;
	if (GetElement(origRow, origCol, &value))
		{
		InsertElement(newRow, newCol, value);
		RemoveElement(origRow, origCol);
		}
}

/******************************************************************************
 GetMaxRowCount

 ******************************************************************************/

JSize
GLRaggedFloatTableData::GetMaxRowCount()
{
	JSize max = 0;
	const JSize colCount = itsCols->GetElementCount();
	for (JSize i = 1; i <= colCount; i++)
		{
		JArray<JFloat>* dataCol = itsCols->GetElement(i);
		if (dataCol->GetElementCount() > max)
			{
			max = dataCol->GetElementCount();
			}
		}
	return max;
}
