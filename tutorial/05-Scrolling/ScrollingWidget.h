/******************************************************************************
 ScrollingWidget.h

	Interface for the ScrollingWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_ScrollingWidget
#define _H_ScrollingWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>

class ScrollingWidget : public JXScrollableWidget
{
public:

	ScrollingWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~ScrollingWidget();
	
protected:
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	
private:

	// not allowed

	ScrollingWidget(const ScrollingWidget& source);
	const ScrollingWidget& operator=(const ScrollingWidget& source);
};

#endif
