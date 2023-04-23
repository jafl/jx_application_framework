/******************************************************************************
 DataTable.h

	Interface for the DataTable class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_DataTable
#define _H_DataTable

#include <jx-af/jx/JXTable.h>

class DataTable : public JXTable
{
public:

	DataTable(JArray<JIndex>* data,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	~DataTable() override;
	
protected:
	
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	
private:

	JArray<JIndex>* itsData;	// we don't own this!
};

#endif
