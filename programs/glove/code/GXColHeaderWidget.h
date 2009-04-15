/******************************************************************************
 GXColHeaderWidget.h

	Interface for the GXColHeaderWidget class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GXColHeaderWidget
#define _H_GXColHeaderWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

private:

	GXRaggedFloatTable*			itsTable;		// we don't own this

private:

	// not allowed

	GXColHeaderWidget(const GXColHeaderWidget& source);
	const GXColHeaderWidget& operator=(const GXColHeaderWidget& source);
};

#endif
