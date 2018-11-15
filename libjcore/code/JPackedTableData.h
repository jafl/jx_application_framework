/******************************************************************************
 JPackedTableData.h

	Interface for JPackedTableData class

	Copyright (C) 1997 by John Lindal & Glenn Bach.

 ******************************************************************************/

#ifndef _H_JPackedTableData
#define _H_JPackedTableData

#include "JTableData.h"
#include "JRunArray.h"

class JTable;

template <class T>
class JPackedTableData : public JTableData
{
public:

	JPackedTableData(const T& defValue);
	JPackedTableData(const JPackedTableData<T>& source);

	virtual ~JPackedTableData();

	const JRunArray< T >&	GetData() const;

	const T	GetElement(const JIndex row, const JIndex col) const;
	const T	GetElement(const JPoint& cell) const;
	void	SetElement(const JIndex row, const JIndex col, const T& data);
	void	SetElement(const JPoint& cell, const T& data);

	void	GetRow(const JIndex index, JList<T>* rowData) const;
	void	SetRow(const JIndex index, const JList<T>& rowData);
	void	SetRow(const JIndex index, const T& data);
	void	SetPartialRow(const JIndex rowIndex,
						  const JIndex firstColIndex, const JIndex lastColIndex,
						  const T& data);

	void	GetCol(const JIndex index, JList<T>* colData) const;
	void	SetCol(const JIndex index, const JList<T>& colData);
	void	SetCol(const JIndex index, const T& data);
	void	SetPartialCol(const JIndex colIndex,
						  const JIndex firstRowIndex, const JIndex lastRowIndex,
						  const T& data);

	void	SetRect(const JRect& rect, const T& data);
	void	SetRect(const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h, const T& data);

	void	ApplyToRect(const JRect& rect, T (*f)(const T&));
	void	ApplyToRect(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h, T (*f)(const T&));

	void	InsertRows(const JIndex index, const JSize count,
					   const JList<T>* initData = nullptr);
	void	PrependRows(const JSize count, const JList<T>* initData = nullptr);
	void	AppendRows(const JSize count, const JList<T>* initData = nullptr);
	void	DuplicateRow(const JIndex origIndex, const JIndex newIndex);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count,
					   const JList<T>* initData = nullptr);
	void	PrependCols(const JSize count, const JList<T>* initData = nullptr);
	void	AppendCols(const JSize count, const JList<T>* initData = nullptr);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveNextCols(const JIndex firstIndex, const JSize count);
	void	RemovePrevCols(const JIndex lastIndex, const JSize count);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	void		SetAllElements(const T& data);
	void		ClearAllElements();
	JBoolean	AllElementsEqual(T* data = nullptr) const;
	JSize		CountElementsWithValue(const T& data) const;

	const T		GetDefaultValue() const;
	void		SetDefaultValue(const T& data);

private:

	JRunArray< T >*	itsData;
	T				itsDefValue;

private:

	JIndex	RCToI(const JIndex row, const JIndex col) const;

	// not allowed

	const JPackedTableData<T>& operator=(const JPackedTableData<T>& source);
};

#include "JPackedTableData.tmpl"

#endif
