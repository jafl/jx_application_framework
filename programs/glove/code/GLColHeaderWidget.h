/******************************************************************************
 GLColHeaderWidget.h

	Interface for the GLColHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GLColHeaderWidget
#define _H_GLColHeaderWidget

#include "JXColHeaderWidget.h"

class GLRaggedFloatTable;

class GLColHeaderWidget : public JXColHeaderWidget
{
public:

	GLColHeaderWidget(GLRaggedFloatTable* table, JXScrollbarSet* scrollbarSet,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GLColHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	GLRaggedFloatTable*			itsTable;		// we don't own this

private:

	// not allowed

	GLColHeaderWidget(const GLColHeaderWidget& source);
	const GLColHeaderWidget& operator=(const GLColHeaderWidget& source);
};

#endif
