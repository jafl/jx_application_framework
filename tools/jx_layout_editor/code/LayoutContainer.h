/******************************************************************************
 LayoutContainer.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutContainer
#define _H_LayoutContainer

#include <jx-af/jx/JXWidget.h>

class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class LayoutDocument;
class LayoutUndo;
class BaseWidget;

class LayoutContainer : public JXWidget
{
public:

	LayoutContainer(LayoutDocument* doc, JXMenuBar* menuBar, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~LayoutContainer() override;

	bool	HasSelection() const;
	void	SelectAllWidgets();
	void	ClearSelection();
	void	GetSelectedWidgets(JPtrArray<BaseWidget>* list) const;
	void	RemoveSelectedWidgets();
	void	Clear(const bool isUndoRedo);

	bool	IsAtLastSaveLocation() const;
	void	SetLastSaveLocation();

	void	AppendEditMenuToToolBar(JXToolBar* toolBar) const;
	void	ReplaceUndo(LayoutUndo* oldUndo, LayoutUndo* newUndo);
	void	SetIgnoreResize(const bool ignore);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	EnclosingBoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDEnter() override;
	void	HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	void	HandleDNDLeave() override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;

private:

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};


private:

	LayoutDocument*	itsDoc;
	JSize			itsGridSpacing;
	Atom			itsLayoutXAtom;

	JXTextMenu*	itsEditMenu;

	JPtrArray<LayoutUndo>*	itsUndoList;
	JIndex					itsFirstRedoIndex;		// range [1:count+1]
	JInteger				itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState				itsUndoState;
	LayoutUndo*				itsResizeUndo;			// nullptr unless windows is resizing
	bool					itsIgnoreResizeFlag;

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;

private:

	void	Undo();
	void	Redo();
	bool	GetCurrentUndo(LayoutUndo** undo) const;
	bool	GetCurrentRedo(LayoutUndo** redo) const;
	void	NewUndo(LayoutUndo* undo);
	void	ClearOutdatedUndo();
	void	ClearUndo();

	void	ClearLastSaveLocation();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
};


/******************************************************************************
 Clear

 ******************************************************************************/

inline void
LayoutContainer::Clear
	(
	const bool isUndoRedo
	)
{
	DeleteEnclosedObjects();

	if (!isUndoRedo)
	{
		ClearUndo();
	}
}

/******************************************************************************
 SetIgnoreResize

 ******************************************************************************/

inline void
LayoutContainer::SetIgnoreResize
	(
	const bool ignore
	)
{
	itsIgnoreResizeFlag = ignore;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline bool
LayoutContainer::IsAtLastSaveLocation()
	const
{
	return (itsLastSaveRedoIndex > 0 &&
			JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex);
}

inline void
LayoutContainer::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
LayoutContainer::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

#endif
