/******************************************************************************
 GLFitParmsTable.h

	Interface for the GLFitParmsTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLFitParmsTable
#define _H_GLFitParmsTable

#include <JXTable.h>
#include <JPtrArray.h>

class JString;

class GLFitParmsTable : public JXTable
{
public:

	GLFitParmsTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~GLFitParmsTable();
	void	Clear();
	void	Append(const JString& col1, const JString& col2);

protected:

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

private:

	JPtrArray<JString>*		itsCol1;
	JPtrArray<JString>*		itsCol2;

private:

	// not allowed

	GLFitParmsTable(const GLFitParmsTable& source);
	const GLFitParmsTable& operator=(const GLFitParmsTable& source);
};


#endif
