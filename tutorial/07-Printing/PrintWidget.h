/******************************************************************************
 PrintWidget.h

	Interface for the PrintWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_PrintWidget
#define _H_PrintWidget

#include <JXScrollableWidget.h>
#include <JArray.h>

class JPainter;
class JPagePrinter;

class PrintWidget : public JXScrollableWidget
{
public:

	PrintWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~PrintWidget();

	void	Print(JPagePrinter& p);

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

private:

	void	DrawStuff(JPainter& p);

	// not allowed

	PrintWidget(const PrintWidget& source);
	const PrintWidget& operator=(const PrintWidget& source);
};

#endif
