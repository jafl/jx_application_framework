/******************************************************************************
 JXMenuTable.h

	Interface for the JXMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuTable
#define _H_JXMenuTable

#include "jx-af/jx/JXTable.h"

class JXMenu;
class JXMenuData;

class JXMenuTable : public JXTable
{
public:

	enum
	{
		kCheckboxColWidth = 20,
		kSubmenuColWidth  = 15,
		kMinRowHeight     = 14
	};

public:

	JXMenuTable(JXMenu* menu, JXMenuData* data, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~JXMenuTable() override;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;
	bool	IsMenuTable() const override;

	// called by JXMenuDirector

	void	GrabKeyboard();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	DrawCheckbox(JPainter& p, const JIndex itemIndex, const JRect& rect);
	void	DrawSubmenuIndicator(JPainter& p, const JIndex itemIndex, const JRect& rect,
								 const bool hilighted);
	void	DrawScrollRegions(JPainter& p);

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	virtual bool	CellToItemIndex(const JPoint& pt, const JPoint& cell,
									JIndex* itemIndex) const = 0;
	virtual void	MenuHilightItem(const JIndex itemIndex) = 0;
	virtual void	MenuUnhilightItem(const JIndex itemIndex) = 0;
	virtual void	GetSubmenuPoints(const JIndex itemIndex,
									 JPoint* leftPt, JPoint* rightPt) = 0;

	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JXMenu*				itsMenu;			// we don't own this
	const JXMenuData*	itsBaseMenuData;

	// used for dragging

	JIndex	itsPrevItemIndex;
	JXMenu*	itsOpenSubmenu;				// we don't own this
	bool	itsSwitchingDragFlag;
	JPoint	itsMouseDownPt;
	JPoint	itsPrevPt;
	JPoint	itsCurrPt;
	bool	itsIsFirstDragFlag;

	bool	itsHasScrollUpFlag;
	bool	itsMouseInScrollUpFlag;
	JRect	itsScrollUpRect;
	JRect	itsScrollUpArrowRect;

	bool	itsHasScrollDownFlag;
	bool	itsMouseInScrollDownFlag;
	JRect	itsScrollDownRect;
	JRect	itsScrollDownArrowRect;

private:

	void	MenuHandleMouseAction(const JPoint& pt,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers,
								  const bool closeIfOutside);
	void	MouseOutsideTable(const JPoint& pt,
							  const JXButtonStates& buttonStates,
							  const JXKeyModifiers& modifiers,
							  JXContainer* trueMouseContainer,
							  const bool shouldClose);
	bool	GetMenuWidgetToActivate(const JPoint& pt, JXContainer** widget);
	bool	CloseMenuOnMouseUp(const JPoint& pt) const;

	void	MenuSelectCell(const JPoint& pt, const JPoint& cell);
	void	MenuSelectItem(const JIndex newItemIndex,
						   const bool checkMovement = true);

	void	UpdateScrollRegions();
	void	ScrollUp(const JCoordinate y);
	void	ScrollDown(const JCoordinate y);
};

#endif
