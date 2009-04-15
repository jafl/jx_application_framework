/******************************************************************************
 GRaggedFloatTableData.cpp

	Class for storing floats in a table of ragged columns.

	BASE CLASS = JTableData

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GRaggedFloatTableData.h"
#include <jAssert.h>

const JCharacter* GRaggedFloatTableData::kElementInserted = "ElementInserted::GRaggedFloatTableData";
const JCharacter* GRaggedFloatTableData::kElementRemoved  = "ElementRemoved::GRaggedFloatTableData";
const JCharacter* GRaggedFloatTableData::kDataChanged     = "DataChanged::GRaggedFloatTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

GRaggedFloatTableData::GRaggedFloatTableData
	(
	const JFloat& defValue
	)
	:
	JTableData(),
	itsDefValue(defValue)
{
	itsCols = new JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != NULL );

	const JSize kInitialColCount = 20;
	const JSize kInitialRowCount = 100;
	ColsAdded(kInitialColCount);
	Broadcast(JTableData::ColsInserted(1, kInitialColCount));

	RowsAdded(kInitialRowCount);
	Broadcast(JTableData::RowsInserted(1, kInitialRowCount));

	itsBroadcast = kJTrue;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

GRaggedFloatTableData::GRaggedFloatTableData
	(
	const GRaggedFloatTableData& source
	)
	:
	JTableData(source),
	itsDefValue(source.itsDefValue),
	itsBroadcast(source.itsBroadcast)
{
	itsCols = new JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != NULL );

	const JSize count = (source.itsCols)->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JArray<JFloat>* origColData = (source.itsCols)->NthElement(i);
		JArray<JFloat>* newColData = new JArray<JFloat>(*origColData);
		assert( newColData != NULL );
		itsCols->Append(newColData);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GRaggedFloatTableData::~GRaggedFloatTableData()
{
	itsCols->DeleteAll();
	delete itsCols;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

JBoolean
GRaggedFloatTableData::GetElement
	(
	const JIndex row,
	const JIndex col,
	JFloat* value
	)
	const
{
	if (CellValid(row,col) == kJFalse)
		{
		return kJFalse;
		}

	const JArray<JFloat>* dataCol = itsCols->NthElement(col);
	*value =  dataCol->GetElement(row);
	return kJTrue;
}

/******************************************************************************
 CreateCellIfNeeded

 ******************************************************************************/

void
GRaggedFloatTableData::CreateCellIfNeeded
	(
	const JIndex row,
	const JIndex col
	)
{
	while (col > itsCols->GetElementCount())
		{
		AppendCol();
		}

	const JBoolean origBroadcast = itsBroadcast;
	if (origBroadcast)
		{
		ShouldBroadcast(kJFalse);
		}

	const JArray<JFloat>* dataCol = itsCols->NthElement(col);
	while (row > dataCol->GetElementCount())
		{
		AppendElement(col, itsDefValue);
		}

	if (origBroadcast)
		{
		ShouldBroadcast(kJTrue);
		}
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
GRaggedFloatTableData::SetElement
	(
	const JIndex	row,
	const JIndex	col,
	const JFloat	data
	)
{
	CreateCellIfNeeded(row, col);
	JArray<JFloat>* dataCol = itsCols->NthElement(col);
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
GRaggedFloatTableData::GetRow
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
		const JArray<JFloat>* dataCol = itsCols->NthElement(i);
		rowData->AppendElement(dataCol->GetElement(index));
		}
}

/******************************************************************************
 SetRow

 ******************************************************************************/

void
GRaggedFloatTableData::SetRow
	(
	const JIndex				index,
	const JArray<JFloat>&	rowData
	)
{
	const JSize colCount = GetDataColCount();
	assert( rowData.GetElementCount() == colCount );

	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* dataCol = itsCols->NthElement(i);
		dataCol->SetElement(index, rowData.GetElement(i));
		}

	Broadcast(JTableData::RectChanged(JRect(index, 1, index+1, colCount+1)));
}

/******************************************************************************
 GetCol

	operator= doesn't work because it could be -any- class derived from JOrderedSet.

 ******************************************************************************/

void
GRaggedFloatTableData::GetCol
	(
	const JIndex	index,
	JOrderedSet<JFloat>*	colData
	)
	const
{
	colData->RemoveAll();

	const JArray<JFloat>* dataCol = itsCols->NthElement(index);
	const JSize rowCount = dataCol->GetElementCount();

	for (JIndex i=1; i<=rowCount; i++)
		{
		colData->InsertElementAtIndex(i, dataCol->GetElement(i));
		}
}

/******************************************************************************
 SetCol

 ******************************************************************************/

void
GRaggedFloatTableData::SetCol
	(
	const JIndex			index,
	const JOrderedSet<JFloat>&	colData
	)
{
	JArray<JFloat>* dataCol = itsCols->NthElement(index);
	const JSize rowCount = colData.GetElementCount();
	CreateCellIfNeeded(rowCount, index);

	for (JIndex i=1; i<=rowCount; i++)
		{
		dataCol->SetElement(i, colData.GetElement(i));
		}

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
GRaggedFloatTableData::InsertRow
	(
	const JIndex				index,
	const JOrderedSet<JFloat>*	initData
	)
{
	assert( initData == NULL || initData->GetElementCount() == GetDataColCount() );

	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->NthElement(i);
		if (index <= colData->GetElementCount())
			{
			JFloat value = 0;
			if ((initData != NULL) && (i <= initData->GetElementCount()))
				{
				value = initData->GetElement(i);
				}
			InsertElement(index, i);
			}
		}
}

/******************************************************************************
 InsertRows (public)

 ******************************************************************************/

void
GRaggedFloatTableData::InsertRows
	(
	const JIndex				index,
	const JSize 				count,
	const JOrderedSet<JFloat>*	initData
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
GRaggedFloatTableData::DuplicateRow
	(
	const JIndex index
	)
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->NthElement(i);
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
GRaggedFloatTableData::RemoveRow
	(
	const JIndex index
	)
{
	assert( index < GetRowCount() );

	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->NthElement(i);
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
GRaggedFloatTableData::RemoveAllRows()
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->NthElement(i);
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
GRaggedFloatTableData::MoveRow
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	const JSize colCount = itsCols->GetElementCount();
	for (JIndex i=1; i<=colCount; i++)
		{
		JArray<JFloat>* colData = itsCols->NthElement(i);
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
GRaggedFloatTableData::InsertCol
	(
	const JIndex			index,
	const JOrderedSet<JFloat>*	initData
	)
{
	JIndex trueIndex = index;
	const JIndex maxIndex = itsCols->GetElementCount()+1;
	if (trueIndex > maxIndex)
		{
		trueIndex = maxIndex;
		}

	JArray<JFloat>* colData = new JArray<JFloat>;
	assert( colData != NULL );
	itsCols->InsertAtIndex(trueIndex, colData);

	if (initData != NULL)
		{
		const JSize rowCount = initData->GetElementCount();
		for (JIndex i=1; i<=rowCount; i++)
			{
			colData->InsertElementAtIndex(i, initData->GetElement(i));
			}
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
GRaggedFloatTableData::InsertCols
	(
	const JIndex				index,
	const JSize 				count,
	const JOrderedSet<JFloat>*	initData
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
GRaggedFloatTableData::DuplicateCol
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

	JArray<JFloat>* origColData = itsCols->NthElement(origIndex);
	JArray<JFloat>* newColData = new JArray<JFloat>(*origColData);
	assert( newColData != NULL );
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
GRaggedFloatTableData::RemoveCol
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
GRaggedFloatTableData::RemoveAllCols()
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
GRaggedFloatTableData::MoveCol
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
GRaggedFloatTableData::InsertElement
	(
	const JIndex row,
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->NthElement(col);
	dataCol->InsertElementAtIndex(row, value);
	const JSize rowCount = dataCol->GetElementCount();
	if (itsBroadcast)
		{
		Broadcast(GRaggedFloatTableData::ElementInserted(row, col));
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
GRaggedFloatTableData::DuplicateElement
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
GRaggedFloatTableData::RemoveElement
	(
	const JIndex row,
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->NthElement(col);
	const JSize rowCount = dataCol->GetElementCount();
	if (row <= rowCount)
		{
		dataCol->RemoveElement(row);
		}
	if (itsBroadcast)
		{
		Broadcast(GRaggedFloatTableData::ElementRemoved(row, col));
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
GRaggedFloatTableData::RemoveAllElements
	(
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->NthElement(col);
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
GRaggedFloatTableData::MoveElement
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
GRaggedFloatTableData::GetMaxRowCount()
{
	JSize max = 0;
	const JSize colCount = itsCols->GetElementCount();
	for (JSize i = 1; i <= colCount; i++)
		{
		JArray<JFloat>* dataCol = itsCols->NthElement(i);
		if (dataCol->GetElementCount() > max)
			{
			max = dataCol->GetElementCount();
			}
		}
	return max;
}
