/******************************************************************************
 FitParmsTable.h

	Interface for the FitParmsTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_FitParmsTable
#define _H_FitParmsTable

#include <JXTable.h>
#include <JPtrArray.h>

class JString;

class FitParmsTable : public JXTable
{
public:

	FitParmsTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~FitParmsTable();
	void	Clear();
	void	Append(const JString& col1, const JString& col2);

protected:

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

private:

	JPtrArray<JString>*		itsCol1;
	JPtrArray<JString>*		itsCol2;

private:

	// not allowed

	FitParmsTable(const FitParmsTable& source);
	const FitParmsTable& operator=(const FitParmsTable& source);
};


#endif
