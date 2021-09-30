/******************************************************************************
 HeaderWidget.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_HeaderWidget
#define _H_HeaderWidget

#include <jx-af/jx/JXColHeaderWidget.h>

class RecordList;

class HeaderWidget : public JXColHeaderWidget
{
public:

	HeaderWidget(JXTable* table, RecordList* list,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~HeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	RecordList*	itsList;
};

#endif
