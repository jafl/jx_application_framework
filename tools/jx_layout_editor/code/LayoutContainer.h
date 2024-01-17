/******************************************************************************
 LayoutContainer.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutContainer
#define _H_LayoutContainer

#include <jx-af/jx/JXWidget.h>
#include "LayoutUndo.h"	// need defn of Type

class JStringManager;
class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class JUndoRedoChain;
class LayoutDocument;
class BaseWidget;

class LayoutContainer : public JXWidget
{
public:

	LayoutContainer(LayoutDocument* doc, JXMenuBar* menuBar, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	LayoutContainer(LayoutContainer* parent, BaseWidget* owner,
					JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~LayoutContainer() override;

	LayoutDocument*	GetDocument() const;
	JXTextMenu*		GetEditMenu() const;
	const JString&	GetCodeTag() const;

	bool	HasSelection() const;
	JSize	GetSelectionCount() const;
	void	SelectAllWidgets();
	void	ClearSelection();
	bool	GetSelectedWidgets(JPtrArray<BaseWidget>* list, const bool all = false) const;
	void	SetSelectedWidgetsVisible(const bool visible);
	void	RemoveSelectedWidgets();
	void	Clear(const bool isUndoRedo);

	void	ReadConfig(std::istream& input, const JFileVersion vers);
	void	WriteConfig(std::ostream& output) const;

	JUndoRedoChain*	GetUndoRedoChain();
	bool			CurrentUndoIs(const LayoutUndo::Type type) const;
	void			NewUndo(LayoutUndo* undo, const bool setChanged = true);
	void			SetIgnoreResize(const bool ignore);

	void	AppendToToolBar(JXToolBar* toolBar) const;

	void	GenerateCode(std::ostream& output, const JString& indent,
						 JPtrArray<JString>* objTypes,
						 JPtrArray<JString>* objNames,
						 JStringManager* stringdb) const;

	const JString&	GetEnclosureName() const;
	JString			GetStringNamespace() const;

	JString GenerateUniqueVarName() const;

	JIndex	GetNextTabIndex() const;
	void	TabIndexRemoved(const JIndex i);

	JSize	GetGridSpacing() const;
	void	SetGridSpacing(const JSize w);
	JPoint	SnapToGrid(const JPoint& pt) const;
	JPoint	SnapToGrid(JXContainer* w) const;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

	static void	WriteWidget(std::ostream& output, const JXContainer* obj,
							JPtrArray<BaseWidget>* widgetList);

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

private:

	LayoutDocument*	itsDoc;

	// both are nullptr if root
	LayoutContainer*	itsParent;
	BaseWidget*			itsOwner;

	JSize	itsGridSpacing;
	Atom	itsLayoutDataXAtom;
	Atom	itsLayoutMetaXAtom;

	JString	itsCodeTag;
	JString	itsWindowTitle;
	JString	itsContainerName;
	bool	itsAdjustContainerToFitFlag;

	JXTextMenu*	itsEditMenu;
	JXTextMenu*	itsLayoutMenu;
	JXTextMenu*	itsArrangeMenu;

	JUndoRedoChain*	itsUndoChain;			// nullptr if not root
	bool			itsIgnoreResizeFlag;

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;

	// used during DND

	JPoint			itsDropPt;
	JPoint			itsDropOffset;
	JArray<JRect>*	itsDropRectList;		// nullptr unless DND
	JPoint			itsBoundsOffset;
	bool			itsCreateDragFlag;
	JRect			itsCreateRect;

private:

	void	LayoutContainerX();

	BaseWidget*	ReadWidget(std::istream& input, const JFileVersion vers,
						   LayoutContainer* defaultEnclosure,
						   JPtrArray<BaseWidget>* widgetList);
	BaseWidget*	CreateWidget(const JIndex index, const JRect& rect);

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);

	void	UpdateLayoutMenu();
	void	HandleLayoutMenu(const JIndex index);

	void	UpdateArrangeMenu();
	void	HandleArrangeMenu(const JIndex index);

	void	AdjustTabOrder(JPtrArray<BaseWidget>* list, const JInteger delta);

	static std::weak_ordering
		CompareLocations(BaseWidget *const w1, BaseWidget *const w2);
	static std::weak_ordering
		CompareTabOrder(BaseWidget *const w1, BaseWidget *const w2);
};


/******************************************************************************
 GetDocument

 ******************************************************************************/

inline LayoutDocument*
LayoutContainer::GetDocument()
	const
{
	return itsDoc;
}

/******************************************************************************
 GetEditMenu

 ******************************************************************************/

inline JXTextMenu*
LayoutContainer::GetEditMenu()
	const
{
	return itsEditMenu;
}

/******************************************************************************
 GetCodeTag

 ******************************************************************************/

inline const JString&
LayoutContainer::GetCodeTag()
	const
{
	return itsParent != nullptr ? itsParent->GetCodeTag() : itsCodeTag;
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
 GetUndoRedoChain

 ******************************************************************************/

inline JUndoRedoChain*
LayoutContainer::GetUndoRedoChain()
{
	return itsParent != nullptr ? itsParent->GetUndoRedoChain() : itsUndoChain;
}

/******************************************************************************
 GetUndoRedoChain

 ******************************************************************************/

inline JSize
LayoutContainer::GetGridSpacing()
	const
{
	return itsGridSpacing;
}

inline void
LayoutContainer::SetGridSpacing
	(
	const JSize w
	)
{
	itsGridSpacing = w;
	Refresh();
}

#endif
