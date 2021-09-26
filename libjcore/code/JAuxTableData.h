/******************************************************************************
 JAuxTableData.h

	Interface for JAuxTableData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAuxTableData
#define _H_JAuxTableData

#include "jx-af/jcore/JPackedTableData.h"

class JTable;

template <class T>
class JAuxTableData : public JPackedTableData<T>
{
public:

	JAuxTableData(JTable* table, const T& defValue);
	JAuxTableData(JTable* table, const JAuxTableData<T>& source);

	virtual ~JAuxTableData();

	JTable*	GetTable() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message);

private:

	JTable*	itsTable;	// not owned

private:

	void	JAuxTableDataX(JTable* table);
	void	AdjustToTable();

	// not allowed

	JAuxTableData(const JAuxTableData<T>&) = delete;
};

#include "JAuxTableData.tmpl"

#endif
