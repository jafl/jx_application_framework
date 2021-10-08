/******************************************************************************
 JObjTableData.h

	Interface for the JObjTableData Class

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#ifndef _H_JObjTableData
#define _H_JObjTableData

#include "jx-af/jcore/JTableData.h"
#include "jx-af/jcore/JPtrArray.h"

template <class T>
class JObjTableData : public JTableData
{
public:

	JObjTableData();
	JObjTableData(const JObjTableData<T>& source);

	~JObjTableData() override;

	const T&	GetElement(const JIndex row, const JIndex col) const;
	const T&	GetElement(const JPoint& cell) const;
	void		SetElement(const JIndex row, const JIndex col, const T& data);
	void		SetElement(const JPoint& cell, const T& data);

	void	GetRow(const JIndex index, JPtrArray<T>* rowData) const;
	void	SetRow(const JIndex index, const JPtrArray<T>& rowData);

	void	GetCol(const JIndex index, JPtrArray<T>* colData) const;
	void	SetCol(const JIndex index, const JPtrArray<T>& colData);

	void	InsertRows(const JIndex index, const JSize count,
					   const JPtrArray<T>* initData = nullptr);
	void	PrependRows(const JSize count, const JPtrArray<T>* initData = nullptr);
	void	AppendRows(const JSize count, const JPtrArray<T>* initData = nullptr);
	void	DuplicateRow(const JIndex origIndex, const JIndex newIndex);
	void	RemoveRow(const JIndex index);
	void	RemoveNextRows(const JIndex firstIndex, const JSize count);
	void	RemovePrevRows(const JIndex lastIndex, const JSize count);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCols(const JIndex index, const JSize count,
					   const JPtrArray<T>* initData = nullptr);
	void	PrependCols(const JSize count, const JPtrArray<T>* initData = nullptr);
	void	AppendCols(const JSize count, const JPtrArray<T>* initData = nullptr);
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

	JObjTableData<T>& operator=(const JObjTableData<T>&) = delete;
};

#include "JObjTableData.tmpl"

#endif
