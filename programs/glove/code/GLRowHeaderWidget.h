/******************************************************************************
 GLRowHeaderWidget.h

	Interface for the GLRowHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GLRowHeaderWidget
#define _H_GLRowHeaderWidget

#include "JXRowHeaderWidget.h"

class GLRaggedFloatTable;

class GLRowHeaderWidget : public JXRowHeaderWidget
{
public:

	GLRowHeaderWidget(GLRaggedFloatTable* table,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GLRowHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
private:

	GLRaggedFloatTable*		itsTable;		// we don't own this

private:

	// not allowed

	GLRowHeaderWidget(const GLRowHeaderWidget& source);
	const GLRowHeaderWidget& operator=(const GLRowHeaderWidget& source);
};

#endif
