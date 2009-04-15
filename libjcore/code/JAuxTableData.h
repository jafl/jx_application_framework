/******************************************************************************
 JAuxTableData.h

	Interface for JAuxTableData class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAuxTableData
#define _H_JAuxTableData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPackedTableData.h>

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

	JAuxTableData(const JAuxTableData<T>& source);
	const JAuxTableData<T>& operator=(const JAuxTableData<T>& source);
};

#endif
