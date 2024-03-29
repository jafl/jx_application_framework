/******************************************************************************
 JValueTableData.h

	Interface for JValueTableData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JValueTableData
#define _H_JValueTableData

#include "JTableData.h"
#include "JPtrArray.h"

template <class S, class T>
class JValueTableData : public JTableData
{
public:

	JValueTableData(const T& defValue);
	JValueTableData(const JValueTableData<S,T>& source);

	~JValueTableData() override;

	T		GetItem(const JIndex row, const JIndex col) const;
	T		GetItem(const JPoint& cell) const;
	void	SetItem(const JIndex row, const JIndex col, const T& data);
	void	SetItem(const JPoint& cell, const T& data);

	void	GetRow(const JIndex index, JList<T>* rowData) const;
	void	SetRow(const JIndex index, const JList<T>& rowData);

	void	GetCol(const JIndex index, JList<T>* colData) const;
	void	SetCol(const JIndex index, const JList<T>& colData);

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

	const T	GetDefaultValue() const;
	void	SetDefaultValue(const T& data);

private:

	JPtrArray< S >*	itsCols;
	T				itsDefValue;

private:

	// not allowed

	JValueTableData<S,T>& operator=(const JValueTableData<S,T>&) = delete;
};

#include "JValueTableData.tmpl"

#endif
