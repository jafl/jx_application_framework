/******************************************************************************
 SimpleTable.h

	Interface for the SimpleTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SimpleTable
#define _H_SimpleTable

#include <jx-af/jx/JXTable.h>

class SimpleTable : public JXTable
{
public:

	SimpleTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);
	
protected:
	
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
};

#endif
