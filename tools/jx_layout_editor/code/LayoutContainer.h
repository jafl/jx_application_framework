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
class JUndoRedoChain;
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

	JUndoRedoChain*	GetUndoRedoChain();
	void			SetIgnoreResize(const bool ignore);

	void	AppendEditMenuToToolBar(JXToolBar* toolBar) const;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	DrawOver(JXWindowPainter& p, const JRect& rect) override;

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
	void	DNDFinish(const bool isDrop, const JXContainer* target) override;

private:

	LayoutDocument*	itsDoc;
	JSize			itsGridSpacing;
	Atom			itsLayoutDataXAtom;
	Atom			itsLayoutMetaXAtom;

	JXTextMenu*	itsEditMenu;

	JUndoRedoChain*	itsUndoChain;
	LayoutUndo*		itsResizeUndo;			// nullptr unless windows is resizing; part of itsUndoList
	bool			itsIgnoreResizeFlag;

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;

	// used during DND

	JPoint			itsDropPt;
	JPoint			itsDropOffset;
	JArray<JRect>*	itsDropRectList;				// nullptr unless DND

private:

	void	NewUndo(LayoutUndo* undo);
	void	ClearUndo();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);
};


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
 GetUndoRedoChain

 ******************************************************************************/

inline JUndoRedoChain*
LayoutContainer::GetUndoRedoChain()
{
	return itsUndoChain;
}

#endif
