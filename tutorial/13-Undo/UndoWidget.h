/******************************************************************************
 UndoWidget.h

	Interface for the UndoWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_UndoWidget
#define _H_UndoWidget

#include <jx-af/jx/JXScrollableWidget.h>

class JUndoRedoChain;
class JUndo;

class UndoWidget : public JXScrollableWidget
{
public:

	friend class UndoLine;
	friend class RedoLine;

public:

	UndoWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~UndoWidget() override;

	void	Undo();
	void	Redo();

	JUndoRedoChain*	GetUndoRedoChain();

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

	JPoint			itsStartPt;
	JPoint			itsPrevPt;
	JArray<JPoint>* itsPoints;

	// used for undo

	JUndoRedoChain*	itsUndoChain;
	
private:

	// needed by undo

	void 	AddLine(const JPoint& start, const JPoint& end);
	void 	RemoveLastLine(); 
};


/******************************************************************************
 Redo (public)

	This is the function that is called when the user asks to redo.

 ******************************************************************************/

inline JUndoRedoChain*
UndoWidget::GetUndoRedoChain()
{
	return itsUndoChain;
}

#endif
