/******************************************************************************
 JAuxTableData.h

	Interface for JAuxTableData class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAuxTableData
#define _H_JAuxTableData

#include "JPackedTableData.h"

class JTable;

template <class T>
class JAuxTableData : public JPackedTableData<T>
{
public:

	JAuxTableData(JTable* table, const T& defValue);
	JAuxTableData(JTable* table, const JAuxTableData<T>& source);

	~JAuxTableData() override;

	JTable*	GetTable() const;

protected:

	void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message) override;

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
