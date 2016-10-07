/******************************************************************************
 Test3DWidget.h

	Interface for the Test3DWidget class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_Test3DWidget
#define _H_Test3DWidget

#include <JX3DWidget.h>

class J3DUniverse;

class Test3DWidget : public JX3DWidget
{
public:

	Test3DWidget(J3DUniverse* universe, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~Test3DWidget();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

private:

	// not allowed

	Test3DWidget(const Test3DWidget& source);
	const Test3DWidget& operator=(const Test3DWidget& source);
};

#endif
