/******************************************************************************
 GMessageColHeader.h

	Interface for the GMessageColHeader class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMessageColHeader
#define _H_GMessageColHeader

#include <JXColHeaderWidget.h>

class JXScrollbar;
class GMMailboxData;

class GMessageColHeader : public JXColHeaderWidget
{
public:

	GMessageColHeader(JXTable* table, JXScrollbarSet* scrollbarSet,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~GMessageColHeader();

	void	SetData(GMMailboxData* data);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

private:

	GMMailboxData*	itsData;

private:

	// not allowed

	GMessageColHeader(const GMessageColHeader& source);
	const GMessageColHeader& operator=(const GMessageColHeader& source);
};
#endif
