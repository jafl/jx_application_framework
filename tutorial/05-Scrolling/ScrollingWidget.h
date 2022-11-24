/******************************************************************************
 ScrollingWidget.h

	Interface for the ScrollingWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_ScrollingWidget
#define _H_ScrollingWidget

#include <jx-af/jx/JXScrollableWidget.h>

class ScrollingWidget : public JXScrollableWidget
{
public:

	ScrollingWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	
protected:
	
	void	Draw(JXWindowPainter& p, const JRect& rect) override;
};

#endif
