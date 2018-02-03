/******************************************************************************
 JXMenuTable.h

	Interface for the JXMenuTable class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenuTable
#define _H_JXMenuTable

#include <JXTable.h>

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

	virtual ~JXMenuTable();

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;
	virtual JBoolean	IsMenuTable() const override;

	// called by JXMenuDirector

	void	GrabKeyboard();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	DrawCheckbox(JPainter& p, const JIndex itemIndex, const JRect& rect);
	void	DrawSubmenuIndicator(JPainter& p, const JIndex itemIndex, const JRect& rect,
								 const JBoolean hilighted);
	void	DrawScrollRegions(JPainter& p);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual JBoolean	CellToItemIndex(const JPoint& pt, const JPoint& cell,
										JIndex* itemIndex) const = 0;
	virtual void		MenuHilightItem(const JIndex itemIndex) = 0;
	virtual void		MenuUnhilightItem(const JIndex itemIndex) = 0;
	virtual void		GetSubmenuPoints(const JIndex itemIndex,
										 JPoint* leftPt, JPoint* rightPt) = 0;

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JXMenu*				itsMenu;			// we don't own this
	const JXMenuData*	itsBaseMenuData;

	// used for dragging

	JIndex		itsPrevItemIndex;
	JXMenu*		itsOpenSubmenu;				// we don't own this
	JBoolean	itsSwitchingDragFlag;
	JPoint		itsMouseDownPt;
	JPoint		itsPrevPt;
	JPoint		itsCurrPt;
	JBoolean	itsIsFirstDragFlag;

	JBoolean	itsHasScrollUpFlag;
	JBoolean	itsMouseInScrollUpFlag;
	JRect		itsScrollUpRect;
	JRect		itsScrollUpArrowRect;

	JBoolean	itsHasScrollDownFlag;
	JBoolean	itsMouseInScrollDownFlag;
	JRect		itsScrollDownRect;
	JRect		itsScrollDownArrowRect;

private:

	void		MenuHandleMouseAction(const JPoint& pt,
									  const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers,
									  const JBoolean closeIfOutside);
	void		MouseOutsideTable(const JPoint& pt,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers,
								  JXContainer* trueMouseContainer,
								  const JBoolean shouldClose);
	JBoolean	GetMenuWidgetToActivate(const JPoint& pt, JXContainer** widget);
	JBoolean	CloseMenuOnMouseUp(const JPoint& pt) const;

	void	MenuSelectCell(const JPoint& pt, const JPoint& cell);
	void	MenuSelectItem(const JIndex newItemIndex,
						   const JBoolean checkMovement = kJTrue);

	void	UpdateScrollRegions();
	void	ScrollUp(const JCoordinate y);
	void	ScrollDown(const JCoordinate y);

	// not allowed

	JXMenuTable(const JXMenuTable& source);
	const JXMenuTable& operator=(const JXMenuTable& source);
};

#endif
