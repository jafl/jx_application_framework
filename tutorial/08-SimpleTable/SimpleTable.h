/******************************************************************************
 SimpleTable.h

	Interface for the SimpleTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SimpleTable
#define _H_SimpleTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTable.h>

class SimpleTable : public JXTable
{
public:

	SimpleTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	virtual ~SimpleTable();
	
protected:
	
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	
private:

	// not allowed

	SimpleTable(const SimpleTable& source);
	const SimpleTable& operator=(const SimpleTable& source);
};

#endif
