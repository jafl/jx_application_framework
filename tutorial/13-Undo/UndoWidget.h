/******************************************************************************
 UndoWidget.h

	Interface for the UndoWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_UndoWidget
#define _H_UndoWidget

#include <jx-af/jx/JXScrollableWidget.h>

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

	// needed by undo
	
	void	Undo();
	void	Redo();
	bool	HasUndo() const;
	bool	HasRedo() const;
	
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

	// needed by undo

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

private:

	// used during drag

	JPoint				itsStartPt;
	JPoint				itsPrevPt;
	JArray<JPoint>* 	itsPoints;

	// used for undo
	
	JPtrArray<JUndo>*	itsUndoList;		
	JIndex				itsFirstRedoIndex;	// range [1:count+1]
	UndoState			itsUndoState;
	
private:

	// needed by undo

	bool 	GetCurrentRedo(JUndo** undo) const;
	bool	GetCurrentUndo(JUndo** undo) const;
	void 	NewUndo(JUndo* undo);
	void 	AddLine(const JPoint& start, const JPoint& end);
	void 	RemoveLastLine(); 
};

#endif
