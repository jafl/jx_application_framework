/******************************************************************************
 JObjTableData.h

	Interface for the JObjTableData Class

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JObjTableData
#define _H_JObjTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTableData.h>
#include <JPtrArray.h>

template <class T>
class JObjTableData : public JTableData
{
public:

	JObjTableData();
	JObjTableData(const JObjTableData<T>& source);

	virtual ~JObjTableData();

	const T&	GetElement(const JIndex row, const JIndex col) const;
	const T&	GetElement(const JPoint& cell) const;
	void		SetElement(const JIndex row, const JIndex col, const T& data);
	void		SetElement(const JPoint& cell, const T& data);

	void	GetRow(const JIndex index, JPtrArray<T>* rowData) const;
	void	SetRow(const JIndex index, const JPtrArray<T>& rowData);

	void	GetCol(const JIndex index, JPtrArray<T>* colData) const;
	void	SetCol(const JIndex index, const JPtrArray<T>& colData);

	void	InsertRows(const JIndex index, const JSize count,
					   const JPtrArray<T>* initData = NULL);
	void	PrependRows(const JSize count, const JPtrArray<T>* initData = NULL);
	void	AppendRows(const JSize count, const JPtrArray<T>* initData = NULL);
	void	DuplicateRow(const JIndex origIndex, const JIndex newIndex);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count,
					   const JPtrArray<T>* initData = NULL);
	void	PrependCols(const JSize count, const JPtrArray<T>* initData = NULL);
	void	AppendCols(const JSize count, const JPtrArray<T>* initData = NULL);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveNextCols(const JIndex firstIndex, const JSize count);
	void	RemovePrevCols(const JIndex lastIndex, const JSize count);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

private:

	JPtrArray< JPtrArray<T> >*	itsCols;

private:

	// not allowed

	const JObjTableData<T>& operator=(const JObjTableData<T>& source);
};

#endif
