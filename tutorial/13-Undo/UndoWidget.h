/******************************************************************************
 UndoWidget.h

	Interface for the UndoWidget class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_UndoWidget
#define _H_UndoWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <JPtrArray.h>

class UndoLine;
class RedoLine;
class JUndo;

class UndoWidget : public JXScrollableWidget
{
public:

	friend UndoLine;
	friend RedoLine;

public:

	UndoWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~UndoWidget();

	// needed by undo
	
	void			Undo();
	void			Redo();
	JBoolean		HasUndo() const;
	JBoolean		HasRedo() const;
	
protected:
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
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

	JBoolean 	GetCurrentRedo(JUndo** undo) const;
	JBoolean	GetCurrentUndo(JUndo** undo) const;
	void 		NewUndo(JUndo* undo);
	void 		AddLine(const JPoint& start, const JPoint& end);
	void 		RemoveLastLine(); 

	// not allowed

	UndoWidget(const UndoWidget& source);
	const UndoWidget& operator=(const UndoWidget& source);
};

#endif
