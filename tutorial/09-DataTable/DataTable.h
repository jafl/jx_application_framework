/******************************************************************************
 DataTable.h

	Interface for the DataTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_DataTable
#define _H_DataTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>
#include <JArray.h>

class DataTable : public JXTable
{
public:

	DataTable(JArray<JIndex>* data,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	virtual ~DataTable();
	
protected:
	
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	
private:

	JArray<JIndex>* itsData;	// we don't own this!

private:

	// not allowed

	DataTable(const DataTable& source);
	const DataTable& operator=(const DataTable& source);
};

#endif
