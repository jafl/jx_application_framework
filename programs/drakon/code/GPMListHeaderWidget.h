/******************************************************************************
 GPMListHeaderWidget.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GPMListHeaderWidget
#define _H_GPMListHeaderWidget

#include <JXColHeaderWidget.h>

class GPMProcessList;

class GPMListHeaderWidget : public JXColHeaderWidget
{
public:

	GPMListHeaderWidget(JXTable* table, GPMProcessList* list,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~GPMListHeaderWidget();

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

	GPMListHeaderWidget(const GPMListHeaderWidget& source);
	const GPMListHeaderWidget& operator=(const GPMListHeaderWidget& source);

};

#endif
