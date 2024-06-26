/******************************************************************************
 JXMenuTable.cpp

	Base class to draw a menu.  We assume nothing about the mapping between
	menu items and table cells.

	To change the scroll steps, change the default row height and column width.

	Inside Draw(), derived classes must call DrawScrollRegions() -last-.

	Derived classes must override the following functions:

		CellToItemIndex
			Convert the given table cell into a menu item index.  Return
			false if the given cell is not a menu item.

		MenuHilightItem
			Hilight the specified menu item.  (Mouse is pointing to it.)

		MenuUnhilightCell
			Unhilight the specified menu item.  (Mouse no longer points to it.)

		GetSubmenuPoints
			Return two appropriate points (in local coordinates) where
			a submenu window might reasonably be placed.  This is usually
			to the right and to the left of the menu item.  The position
			to the right will be preferred.

	BASE CLASS = JXTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXMenuTable.h"
#include "JXMenuData.h"
#include "JXMenuManager.h"
#include "JXMenuDirector.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXWindowPainter.h"
#include "JXColorManager.h"
#include "jXPainterUtil.h"
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JSize kFastScrollFactor    = 3;
const JCoordinate kMoveSlowDelta = 3;

const JCoordinate kCheckboxHalfHeight = 5;	// remember to update kMinRowHeight
const JCoordinate kRadioboxHalfHeight = 5;
const JCoordinate kScrollRectSize     = 17;

const JRect kSubmenuArrowRect(-4, -1, 5, 4);
const JRect kScrollUpArrowRect(-4, -7, 4, 8);
const JRect kScrollDownArrowRect(-4, -8, 4, 9);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenuTable::JXMenuTable
	(
	JXMenu*				menu,
	JXMenuData*			data,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTable(20,20, nullptr, enclosure, hSizing,vSizing, x,y, w,h),
	itsMenu( menu ),
	itsBaseMenuData( data )
{
	assert( menu != nullptr && data != nullptr );

	itsPrevItemIndex = 0;
	itsOpenSubmenu   = nullptr;

	itsSwitchingDragFlag = false;
	itsIsFirstDragFlag   = true;

	itsHasScrollUpFlag       = false;
	itsMouseInScrollUpFlag   = false;
	itsHasScrollDownFlag     = false;
	itsMouseInScrollDownFlag = false;

	WantInput(true, true, true);		// we don't have a scrollbar set

	SetDrawOrder(kDrawByRow);
	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());

	SetDefaultCursor(itsMenu->GetDefaultCursor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMenuTable::~JXMenuTable()
{
	if (itsOpenSubmenu != nullptr)
	{
		itsOpenSubmenu->Close();
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXMenuTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXTable::Draw(p, rect);
	DrawScrollRegions(p);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXMenuTable::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawUpFrame(p, frame, GetBorderWidth());
}

/******************************************************************************
 DrawCheckbox (protected)

 ******************************************************************************/

void
JXMenuTable::DrawCheckbox
	(
	JPainter&		p,
	const JIndex	itemIndex,
	const JRect&	rect
	)
{
	const JXMenu::ItemType type = itsBaseMenuData->GetType(itemIndex);

	if (type == JXMenu::kCheckboxType || type == JXMenu::kRadioType)
	{
		const bool isChecked = itsBaseMenuData->IsChecked(itemIndex);

		const JPoint center(rect.xcenter(), rect.ycenter());
		JRect boxRect(center, center);
		(boxRect.bottom)--;
		(boxRect.right)--;

		const JColorID selColor = JColorManager::GetDefaultSelButtonColor();

		if (type == JXMenu::kRadioType)
		{
			boxRect.Shrink(-kRadioboxHalfHeight, -kRadioboxHalfHeight);
			if (isChecked)
			{
				JXDrawDownDiamond(p, boxRect, kJXDefaultBorderWidth, true, selColor);
			}
			else
			{
				JXDrawUpDiamond(p, boxRect, kJXDefaultBorderWidth);
			}
		}
		else
		{
			assert( type == JXMenu::kCheckboxType );

			boxRect.Shrink(-kCheckboxHalfHeight, -kCheckboxHalfHeight);
			if (isChecked)
			{
				JXDrawDownFrame(p, boxRect, kJXDefaultBorderWidth, true, selColor);
			}
			else
			{
				JXDrawUpFrame(p, boxRect, kJXDefaultBorderWidth);
			}
		}
	}
}

/******************************************************************************
 DrawSubmenuIndicator (protected)

 ******************************************************************************/

void
JXMenuTable::DrawSubmenuIndicator
	(
	JPainter&		p,
	const JIndex	itemIndex,
	const JRect&	rect,
	const bool	hilighted
	)
{
	if (itsBaseMenuData->HasSubmenu(itemIndex))
	{
		JRect r = kSubmenuArrowRect;
		r.Shift(rect.center());

		const JColorID colorIndex =
			itsBaseMenuData->IsEnabled(itemIndex) ?
				JColorManager::GetGrayColor(40) :
				JColorManager::GetInactiveLabelColor();
		JXFillArrowRight(p, r, colorIndex);

//		const JRect kSubmenuArrowRect(-4, -5, 5, 5);
/*		if (hilighted)
		{
			JXDrawDownArrowRight(p, r, 1);
		}
		else
		{
			JXDrawUpArrowRight(p, r, 1);
		}
*/		}
}

/******************************************************************************
 DrawScrollRegions (protected)

 ******************************************************************************/

void
JXMenuTable::DrawScrollRegions
	(
	JPainter& p
	)
{
	if (itsHasScrollUpFlag && itsMouseInScrollUpFlag)
	{
		JXDrawUpFrame(p, itsScrollUpRect, kJXDefaultBorderWidth,
					  true, GetCurrBackColor());
		JXDrawDownArrowUp(p, itsScrollUpArrowRect, 1);
	}
	else if (itsHasScrollUpFlag)
	{
		p.SetPenColor(GetCurrBackColor());
		p.SetFilling(true);
		p.Rect(itsScrollUpRect);
		JXDrawUpArrowUp(p, itsScrollUpArrowRect, 1);
	}

	if (itsHasScrollDownFlag && itsMouseInScrollDownFlag)
	{
		JXDrawUpFrame(p, itsScrollDownRect, kJXDefaultBorderWidth,
					  true, GetCurrBackColor());
		JXDrawDownArrowDown(p, itsScrollDownArrowRect, 1);
	}
	else if (itsHasScrollDownFlag)
	{
		p.SetPenColor(GetCurrBackColor());
		p.SetFilling(true);
		p.Rect(itsScrollDownRect);
		JXDrawUpArrowDown(p, itsScrollDownArrowRect, 1);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXMenuTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsSwitchingDragFlag = false;

	GetWindow()->GrabPointer(this);

	itsMouseDownPt = pt;
	itsPrevPt      = JPoint(0,0);
	MenuHandleMouseAction(pt, buttonStates, modifiers, true);

/*  This can't work because the mouse is grabbed by the menu's owner.

	if (button == kJXButton4 || button == kJXButton5)
	{
		const JCoordinate sign  = (button == kJXButton4 ? -1 : +1);
		const JCoordinate delta = sign * GetDefaultRowHeight();
		if (modifiers.shift())
		{
			Scroll(0, delta);
		}
		else if (modifiers.control())
		{
			Scroll(0, sign * (GetBoundsHeight() - GetDefaultRowHeight()));
		}
		else
		{
			Scroll(0, kWheelLineCount * delta);
		}
	}
*/
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXMenuTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	MenuHandleMouseAction(pt, buttonStates, modifiers, false);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	We use itsPrevItemIndex because that is what the user saw last.

 ******************************************************************************/

void
JXMenuTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (!itsSwitchingDragFlag && itsPrevItemIndex > 0)
	{
		if (!itsBaseMenuData->HasSubmenu(itsPrevItemIndex))
		{
			itsMenu->BroadcastSelection(itsPrevItemIndex, false);	// destroys us
		}
		else
		{
			itsIsFirstDragFlag = false;
		}
	}
	else if (!itsSwitchingDragFlag && CloseMenuOnMouseUp(pt))
	{
		GetMenuManager()->CloseCurrentMenus();	// destroys us
	}
	else
	{
		// We need a separate clause because the other clauses may delete
		// the object.  We can't modify instance variables after that.

		itsIsFirstDragFlag = false;
	}
}

// private

bool
JXMenuTable::CloseMenuOnMouseUp
	(
	const JPoint& pt
	)
	const
{
	if (itsIsFirstDragFlag && itsMenu->IsHiddenPopupMenu() &&
		!JMouseMoved(itsMouseDownPt, pt))
	{
		return false;
	}

	JXDisplay* display = GetDisplay();
	JXContainer* widget;
	if (!display->FindMouseContainer(this, pt, &widget) ||
		widget != itsMenu)
	{
//		for debugging:
//		XUngrabKeyboard(*(GetDisplay()), CurrentTime);
//		XUngrabPointer(*(GetDisplay()), CurrentTime);
//		GetDisplay()->Flush();

		return true;
	}

	JXContainer* origWidget;
	return !itsIsFirstDragFlag &&
		display->FindMouseContainer(this, itsMouseDownPt, &origWidget) &&
		origWidget == itsMenu;
}

/******************************************************************************
 MenuHandleMouseAction (private)

 ******************************************************************************/

void
JXMenuTable::MenuHandleMouseAction
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const bool				closeIfOutside
	)
{
	itsCurrPt = pt;

	// check if mouse is moving towards submenu
	// (The user perceives this as a mode because when he is aiming for the
	//  submenu, nothing else on the screen matters.)

	if (itsOpenSubmenu != nullptr &&
		itsOpenSubmenu->itsMenuDirector != nullptr)
	{
		JXWindow* w          = GetWindow();
		const JPoint currPtR = w->GlobalToRoot(w->LocalToGlobal(itsCurrPt));

		JXWindow* subw   = (itsOpenSubmenu->itsMenuDirector)->GetWindow();
		const JRect subr = subw->GlobalToRoot(subw->GetBoundsGlobal());

		const JFloat s = JFloat(itsCurrPt.y - itsPrevPt.y)/
						 JFloat(itsCurrPt.x - itsPrevPt.x);

		bool movingTowardsSubmenu = false;
		if (itsPrevPt.x < itsCurrPt.x && currPtR.x < subr.left)
		{
			const JFloat y       = currPtR.y + s * (subr.left - currPtR.x);
			movingTowardsSubmenu = subr.top < y && y < subr.bottom;
		}
		else if (subr.right < currPtR.x && itsCurrPt.x < itsPrevPt.x)
		{
			const JFloat y       = currPtR.y + s * (subr.right - currPtR.x);
			movingTowardsSubmenu = subr.top < y && y < subr.bottom;
		}

		if (movingTowardsSubmenu)
		{
			itsPrevPt = pt;
			return;
		}
	}

	// check if mouse has moved into other window (submenu) on top of us

	JXContainer* widget;
	if (GetMenuWidgetToActivate(pt, &widget))
	{
		JXDisplay* display = GetDisplay();	// need local copy, since we might be deleted
		Display* xDisplay  = *display;
		Window xWindow     = GetWindow()->GetXWindow();

		GetWindow()->UngrabPointer(this);
		itsSwitchingDragFlag = true;
		display->SwitchDrag(this, pt, buttonStates, modifiers, widget);	// can destroy us
		if (JXDisplay::WindowExists(display, xDisplay, xWindow))
		{
			itsSwitchingDragFlag = false;
		}
		return;
	}

	// process the event ourselves

	if (itsHasScrollUpFlag)
	{
		bool flag = itsScrollUpRect.Contains(pt) || pt.y <= itsScrollUpRect.top;
		if (flag != itsMouseInScrollUpFlag)
		{
			RefreshRect(itsScrollUpRect);
		}
		itsMouseInScrollUpFlag = flag;
	}

	if (itsHasScrollDownFlag)
	{
		bool flag = itsScrollDownRect.Contains(pt) || pt.y >= itsScrollDownRect.bottom;
		if (flag != itsMouseInScrollDownFlag)
		{
			RefreshRect(itsScrollDownRect);
		}
		itsMouseInScrollDownFlag = flag;
	}

	JPoint cell;
	if (itsMouseInScrollUpFlag)
	{
		MenuSelectItem(0);
		ScrollUp(pt.y);
	}
	else if (itsMouseInScrollDownFlag)
	{
		MenuSelectItem(0);
		ScrollDown(pt.y);
	}
	else if (GetCell(pt, &cell))
	{
		MenuSelectCell(pt, cell);
	}
	else
	{
		MouseOutsideTable(pt, buttonStates, modifiers, widget, closeIfOutside);
	}

	itsPrevPt = pt;
}

/******************************************************************************
 MouseOutsideTable (private)

 ******************************************************************************/

void
JXMenuTable::MouseOutsideTable
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JXContainer*			trueMouseContainer,
	const bool			shouldClose
	)
{
	if (itsHasScrollUpFlag && pt.y <= itsScrollUpRect.top)
	{
		if (!itsMouseInScrollUpFlag)
		{
			RefreshRect(itsScrollUpRect);
		}
		itsMouseInScrollUpFlag = true;
		MenuSelectItem(0);
		ScrollUp(pt.y);
	}
	else if (itsHasScrollDownFlag && pt.y >= itsScrollDownRect.bottom)
	{
		if (!itsMouseInScrollDownFlag)
		{
			RefreshRect(itsScrollDownRect);
		}
		itsMouseInScrollDownFlag = true;
		MenuSelectItem(0);
		ScrollDown(pt.y);
	}
	else if (shouldClose && trueMouseContainer != itsMenu && trueMouseContainer != this)
	{
		GetMenuManager()->CloseCurrentMenus();	// destroys us
	}
	else
	{
		MenuSelectItem(0);
	}
}

/******************************************************************************
 GetMenuWidgetToActivate (private)

	Returns true if the mouse is in a widget that is part of the same
	menu bar as origMenu.

 ******************************************************************************/

bool
JXMenuTable::GetMenuWidgetToActivate
	(
	const JPoint&	pt,
	JXContainer**	widget
	)
{
	if (!GetDisplay()->FindMouseContainer(this, pt, widget))
	{
		*widget = nullptr;
		return false;
	}

	if ((**widget).IsMenuTable() && *widget != this)
	{
		return true;
	}

	if ((**widget).IsMenu())
	{
		auto& menu = dynamic_cast<JXMenu&>(**widget);

		JXMenuBar* menuBar1;
		JXMenuBar* menuBar2;
		if (menu.GetMenuBar(&menuBar1) && itsMenu->GetMenuBar(&menuBar2) &&
			menuBar1 == menuBar2 &&
			menu.GetTopLevelMenu() != itsMenu->GetTopLevelMenu())
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 MenuSelectCell (private)

 ******************************************************************************/

void
JXMenuTable::MenuSelectCell
	(
	const JPoint& pt,
	const JPoint& cell
	)
{
	JIndex itemIndex;
	if (CellToItemIndex(pt, cell, &itemIndex))
	{
		MenuSelectItem(itemIndex);
	}
	else
	{
		MenuSelectItem(0);
	}
}

/******************************************************************************
 MenuSelectItem (private)

 ******************************************************************************/

void
JXMenuTable::MenuSelectItem
	(
	const JIndex	origNewItemIndex,
	const bool	checkMovement
	)
{
	JIndex newItemIndex = origNewItemIndex;
	if (newItemIndex > 0 && !itsBaseMenuData->IsEnabled(newItemIndex))
	{
		newItemIndex = 0;
	}

	JXMenu* submenu = nullptr;
	if (newItemIndex != 0)
	{
		itsBaseMenuData->GetSubmenu(newItemIndex, &submenu);
	}

	if (newItemIndex == itsPrevItemIndex &&
		(submenu == nullptr || submenu->IsOpen()))
	{
		return;
	}

	if (itsPrevItemIndex != 0)
	{
		MenuUnhilightItem(itsPrevItemIndex);
		if (itsOpenSubmenu != nullptr)
		{
			itsOpenSubmenu->Close();
			itsOpenSubmenu = nullptr;
			GrabKeyboard();
		}
	}

	if (newItemIndex != 0)
	{
		MenuHilightItem(newItemIndex);
		GetWindow()->Update();

		if (submenu != nullptr &&
			(!checkMovement ||
			 (labs(itsCurrPt.x - itsPrevPt.x) <= kMoveSlowDelta &&
			  labs(itsCurrPt.y - itsPrevPt.y) <= kMoveSlowDelta)))
		{
			if (itsOpenSubmenu != nullptr)
			{
				itsOpenSubmenu->Close();
				itsOpenSubmenu = nullptr;
			}

			JPoint leftPt, rightPt;
			GetSubmenuPoints(newItemIndex, &leftPt, &rightPt);

			JXWindow* window      = GetWindow();
			const JPoint leftPtG  = JXContainer::LocalToGlobal(leftPt);
			const JPoint leftPtR  = window->GlobalToRoot(leftPtG);
			const JPoint rightPtG = JXContainer::LocalToGlobal(rightPt);
			const JPoint rightPtR = window->GlobalToRoot(rightPtG);
			if (submenu->Open(leftPtR, rightPtR))
			{
				itsOpenSubmenu = submenu;
			}
		}
	}

	itsPrevItemIndex = newItemIndex;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXMenuTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c.IsSpace() || c == kJEscapeKey)
	{
		GetMenuManager()->CloseCurrentMenus();		// destroys us
		return;
	}

	JIndex index;
	const bool isShortcut = itsBaseMenuData->ShortcutToIndex(c, &index);
	if (isShortcut && !itsBaseMenuData->HasSubmenu(index))
	{
		itsMenu->BroadcastSelection(index, false);		// destroys us
	}
	else if (isShortcut)
	{
		MenuSelectItem(index, false);
	}
}

/******************************************************************************
 GrabKeyboard

 ******************************************************************************/

void
JXMenuTable::GrabKeyboard()
{
	XUngrabKeyboard(*GetDisplay(), CurrentTime);

	XGrabKeyboard(*GetDisplay(), GetWindow()->GetXWindow(), False,
				  GrabModeAsync, GrabModeAsync, CurrentTime);

	GetDisplay()->SetKeyboardGrabber(GetWindow());
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Update our scroll arrows.

 ******************************************************************************/

void
JXMenuTable::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXTable::BoundsMoved(dx,dy);
	UpdateScrollRegions();
}

/******************************************************************************
 BoundsResized (virtual protected)

	Update our scroll arrows.

 ******************************************************************************/

void
JXMenuTable::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::BoundsResized(dw,dh);
	UpdateScrollRegions();
}

/******************************************************************************
 ApertureResized (virtual protected)

	Update our scroll arrows.

 ******************************************************************************/

void
JXMenuTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	UpdateScrollRegions();
}

/******************************************************************************
 UpdateScrollRegions (private)

	Calculate the regions for the scroll arrows.

 ******************************************************************************/

void
JXMenuTable::UpdateScrollRegions()
{
	const JRect ap     = GetAperture();
	const JRect bounds = GetBounds();

	itsHasScrollUpFlag = bounds.top < ap.top;
	if (itsHasScrollUpFlag)
	{
		itsScrollUpRect = ap;
		itsScrollUpRect.bottom = ap.top + kScrollRectSize;

		itsScrollUpArrowRect = kScrollUpArrowRect;
		itsScrollUpArrowRect.Shift(itsScrollUpRect.center());
	}
	else
	{
		itsMouseInScrollUpFlag = false;
	}

	itsHasScrollDownFlag = bounds.bottom > ap.bottom;
	if (itsHasScrollDownFlag)
	{
		itsScrollDownRect = ap;
		itsScrollDownRect.top = ap.bottom - kScrollRectSize;

		itsScrollDownArrowRect = kScrollDownArrowRect;
		itsScrollDownArrowRect.Shift(itsScrollDownRect.center());
	}
	else
	{
		itsMouseInScrollDownFlag = false;
	}
}

/******************************************************************************
 ScrollUp (private)

 ******************************************************************************/

void
JXMenuTable::ScrollUp
	(
	const JCoordinate y
	)
{
	JCoordinate dy = GetDefaultRowHeight();
	if (y < itsScrollUpRect.top + kJXDefaultBorderWidth)
	{
		dy *= kFastScrollFactor;
	}

	Scroll(0, dy);
}

/******************************************************************************
 ScrollDown (private)

 ******************************************************************************/

void
JXMenuTable::ScrollDown
	(
	const JCoordinate y
	)
{
	JCoordinate dy = GetDefaultRowHeight();
	if (y > itsScrollDownRect.bottom - kJXDefaultBorderWidth)
	{
		dy *= kFastScrollFactor;
	}

	Scroll(0, -dy);
}

/******************************************************************************
 Menu ID routine (virtual)

 ******************************************************************************/

bool
JXMenuTable::IsMenuTable()
	const
{
	return true;
}
