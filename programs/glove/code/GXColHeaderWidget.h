/******************************************************************************
 GXColHeaderWidget.h

	Interface for the GXColHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GXColHeaderWidget
#define _H_GXColHeaderWidget

#include "JXColHeaderWidget.h"

class GXRaggedFloatTable;

class GXColHeaderWidget : public JXColHeaderWidget
{
public:

	GXColHeaderWidget(GXRaggedFloatTable* table, JXScrollbarSet* scrollbarSet,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GXColHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	GXRaggedFloatTable*			itsTable;		// we don't own this

private:

	// not allowed

	GXColHeaderWidget(const GXColHeaderWidget& source);
	const GXColHeaderWidget& operator=(const GXColHeaderWidget& source);
};

#endif
