/******************************************************************************
 GXRowHeaderWidget.h

	Interface for the GXRowHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GXRowHeaderWidget
#define _H_GXRowHeaderWidget

#include "JXRowHeaderWidget.h"

class GXRaggedFloatTable;

class GXRowHeaderWidget : public JXRowHeaderWidget
{
public:

	GXRowHeaderWidget(GXRaggedFloatTable* table,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GXRowHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
private:

	GXRaggedFloatTable*		itsTable;		// we don't own this

private:

	// not allowed

	GXRowHeaderWidget(const GXRowHeaderWidget& source);
	const GXRowHeaderWidget& operator=(const GXRowHeaderWidget& source);
};

#endif
