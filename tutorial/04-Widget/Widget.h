/******************************************************************************
 Widget.h

	Interface for the Widget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_Widget
#define _H_Widget

#include <JXWidget.h>

class Widget : public JXWidget
{
public:

	Widget(JXContainer* enclosure,
		   const HSizingOption hSizing, const VSizingOption vSizing,
		   const JCoordinate x, const JCoordinate y,
		   const JCoordinate w, const JCoordinate h);

	virtual ~Widget();
	
protected:
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	
private:

	// not allowed

	Widget(const Widget& source);
	const Widget& operator=(const Widget& source);
};

#endif
