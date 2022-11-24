/******************************************************************************
 Widget.h

	Interface for the Widget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_Widget
#define _H_Widget

#include <jx-af/jx/JXWidget.h>

class Widget : public JXWidget
{
public:

	Widget(JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);
	
protected:
	
	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
