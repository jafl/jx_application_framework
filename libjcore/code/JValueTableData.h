/******************************************************************************
 JValueTableData.h

	Interface for JValueTableData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JValueTableData
#define _H_JValueTableData

#include <JTableData.h>
#include <JPtrArray.h>

template <class S, class T>
class JValueTableData : public JTableData
{
public:

	JValueTableData(const T& defValue);
	JValueTableData(const JValueTableData<S,T>& source);

	virtual ~JValueTableData();

	const T	GetElement(const JIndex row, const JIndex col) const;
	const T	GetElement(const JPoint& cell) const;
	void	SetElement(const JIndex row, const JIndex col, const T& data);
	void	SetElement(const JPoint& cell, const T& data);

	void	GetRow(const JIndex index, JList<T>* rowData) const;
	void	SetRow(const JIndex index, const JList<T>& rowData);

	void	GetCol(const JIndex index, JList<T>* colData) const;
	void	SetCol(const JIndex index, const JList<T>& colData);

	void	InsertRows(const JIndex index, const JSize count,
					   const JList<T>* initData = NULL);
	void	PrependRows(const JSize count, const JList<T>* initData = NULL);
	void	AppendRows(const JSize count, const JList<T>* initData = NULL);
	void	DuplicateRow(const JIndex origIndex, const JIndex newIndex);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count,
					  const JList<T>* initData = NULL);
	void	PrependCols(const JSize count, const JList<T>* initData = NULL);
	void	AppendCols(const JSize count, const JList<T>* initData = NULL);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveNextCols(const JIndex firstIndex, const JSize count);
	void	RemovePrevCols(const JIndex lastIndex, const JSize count);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	const T	GetDefaultValue() const;
	void	SetDefaultValue(const T& data);

private:

	JPtrArray< S >*	itsCols;
	T				itsDefValue;

private:

	// not allowed

	const JValueTableData<S,T>& operator=(const JValueTableData<S,T>& source);
};

#include <JValueTableData.tmpl>

#endif
