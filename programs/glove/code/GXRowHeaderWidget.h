/******************************************************************************
 GXRowHeaderWidget.h

	Interface for the GXRowHeaderWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXRowHeaderWidget
#define _H_GXRowHeaderWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
									const JXKeyModifiers& modifiers);
private:

	GXRaggedFloatTable*		itsTable;		// we don't own this

private:

	// not allowed

	GXRowHeaderWidget(const GXRowHeaderWidget& source);
	const GXRowHeaderWidget& operator=(const GXRowHeaderWidget& source);
};

#endif
