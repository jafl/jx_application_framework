/******************************************************************************
 DragWidget.h

	Interface for the DragWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_DragWidget
#define _H_DragWidget

#include <JXScrollableWidget.h>
#include <JArray.h>

class DragWidget : public JXScrollableWidget
{
public:

	DragWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~DragWidget();

	virtual void	HandleKeyPress(const int key,				
								   const JXKeyModifiers& modifiers) override;
	
protected:
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

private:

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;
	
	JArray<JPoint>* itsPoints;
	
private:

	// not allowed

	DragWidget(const DragWidget& source);
	const DragWidget& operator=(const DragWidget& source);
};

#endif
