/******************************************************************************
 GPMTreeHeaderWidget.h

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#ifndef _H_GPMTreeHeaderWidget
#define _H_GPMTreeHeaderWidget

#include <JXColHeaderWidget.h>

class GPMProcessList;

class GPMTreeHeaderWidget : public JXColHeaderWidget
{
public:

	GPMTreeHeaderWidget(JXTable* table, GPMProcessList* list,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~GPMTreeHeaderWidget();

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	GPMProcessList*	itsList;

private:

	// not allowed

	GPMTreeHeaderWidget(const GPMTreeHeaderWidget& source);
	const GPMTreeHeaderWidget& operator=(const GPMTreeHeaderWidget& source);

};

#endif
