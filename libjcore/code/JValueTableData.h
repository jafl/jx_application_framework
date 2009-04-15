/******************************************************************************
 JValueTableData.h

	Interface for JValueTableData class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JValueTableData
#define _H_JValueTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	void	GetRow(const JIndex index, JOrderedSet<T>* rowData) const;
	void	SetRow(const JIndex index, const JOrderedSet<T>& rowData);

	void	GetCol(const JIndex index, JOrderedSet<T>* colData) const;
	void	SetCol(const JIndex index, const JOrderedSet<T>& colData);

	void	InsertRows(const JIndex index, const JSize count,
					   const JOrderedSet<T>* initData = NULL);
	void	PrependRows(const JSize count, const JOrderedSet<T>* initData = NULL);
	void	AppendRows(const JSize count, const JOrderedSet<T>* initData = NULL);
	void	DuplicateRow(const JIndex origIndex, const JIndex newIndex);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count,
					  const JOrderedSet<T>* initData = NULL);
	void	PrependCols(const JSize count, const JOrderedSet<T>* initData = NULL);
	void	AppendCols(const JSize count, const JOrderedSet<T>* initData = NULL);
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

#endif
