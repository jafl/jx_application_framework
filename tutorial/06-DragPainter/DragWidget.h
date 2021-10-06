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

	void	HandleKeyPress(const int key,				
								   const JXKeyModifiers& modifiers) override;
	
protected:
	
	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

private:

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;
	
	JArray<JPoint>* itsPoints;
};

#endif
