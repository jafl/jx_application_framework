/******************************************************************************
 MDHeaderWidget.h

	Copyright (C) 2010 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_MDHeaderWidget
#define _H_MDHeaderWidget

#include <JXColHeaderWidget.h>

class MDRecordList;

class MDHeaderWidget : public JXColHeaderWidget
{
public:

	MDHeaderWidget(JXTable* table, MDRecordList* list,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~MDHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

private:

	MDRecordList*	itsList;

private:

	// not allowed

	MDHeaderWidget(const MDHeaderWidget& source);
	const MDHeaderWidget& operator=(const MDHeaderWidget& source);

};

#endif
