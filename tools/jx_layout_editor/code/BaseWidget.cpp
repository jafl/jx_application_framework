/******************************************************************************
 BaseWidget.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "BaseWidget.h"
#include "LayoutContainer.h"
#include "LayoutSelection.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

BaseWidget::BaseWidget
	(
	LayoutContainer*	layout,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsLayout(layout),
	itsMemberVarFlag(false),
	itsSelectedFlag(false)
{
	itsVarName = itsLayout->GenerateUniqueVarName();
}

BaseWidget::BaseWidget
	(
	LayoutContainer*	layout,
	std::istream&		input,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsLayout(layout),
	itsSelectedFlag(false)
{
	input >> itsVarName >> itsMemberVarFlag;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BaseWidget::~BaseWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

	Remember to keep LayoutDocument::ReadFile in sync!

 ******************************************************************************/

void
BaseWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	// read by LayoutDocument

	output << (int) GetHSizing() << std::endl;
	output << (int) GetVSizing() << std::endl;
	output << GetFrame() << std::endl;

	// read by ctor

	output << itsVarName << std::endl;
	output << itsMemberVarFlag << std::endl;
}

/******************************************************************************
 DrawSelection (protected)

 ******************************************************************************/

void
BaseWidget::DrawSelection
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsSelectedFlag)
	{
		p.SetPenColor(JColorManager::GetDefaultSelectionColor());
		p.Rect(rect);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPt = itsPrevPt = pt;
	itsLastClickCount      = 0;
	itsWaitingForDragFlag  = false;
	itsClearIfNotDNDFlag   = false;

	if (button == kJXLeftButton && clickCount == 1 && modifiers.shift())
	{
		if (!IsSelected())
		{
			JPtrArray<BaseWidget> list(JPtrArrayT::kForgetAll);
			if (!itsLayout->GetSelectedWidgets(&list) ||
				list.GetFirstItem()->GetEnclosure() == GetEnclosure())
			{
				SetSelected(true);
			}
		}
		else if (IsSelected())
		{
			SetSelected(false);
		}
	}
	else if (button == kJXLeftButton && clickCount > 1)
	{
		itsWaitingForDragFlag = itsLayout->HasSelection();
		itsLastClickCount     = clickCount;		// save for HandleMouseUp()
	}
	else if (button == kJXLeftButton)
	{
		itsWaitingForDragFlag = true;

		if (itsSelectedFlag)
		{
			itsClearIfNotDNDFlag = true;
		}
		else
		{
			itsLayout->ClearSelection();
			SetSelected(true);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsWaitingForDragFlag)
	{
		assert( itsLayout->HasSelection() );

		if (JMouseMoved(itsStartPt, pt))
		{
			itsWaitingForDragFlag = false;
			itsClearIfNotDNDFlag  = false;

			itsLayout->SetSelectedWidgetsVisible(false);

			auto* data = jnew LayoutSelection(itsLayout, LocalToGlobal(itsStartPt));
			BeginDND(pt, buttonStates, modifiers, data);
		}
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
BaseWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsWaitingForDragFlag && itsLastClickCount == 2)
	{
		// TODO:  edit parameters
	}
	else if (itsClearIfNotDNDFlag)
	{
		itsLayout->ClearSelection();
		SetSelected(true);
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
BaseWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JXMenu::Style style = JXMenu::GetDisplayStyle();
	bool move =
		(style == JXMenu::kMacintoshStyle && modifiers.meta()) ||
		(style == JXMenu::kWindowsStyle   && modifiers.control());

	if (target == itsLayout)
	{
		move = !move;
	}

	itsLayout->SetSelectedWidgetsVisible(!move);

	return (move ?
			GetDNDManager()->GetDNDActionMoveXAtom() : 
			GetDNDManager()->GetDNDActionCopyXAtom());
}

/******************************************************************************
 DNDFinish (virtual protected)

	This is called when DND is terminated, but before the actual data
	transfer.

	If it is not a drop, or the drop target is not within the same
	application, target is nullptr.

 ******************************************************************************/

void
BaseWidget::DNDFinish
	(
	const bool			isDrop,
	const JXContainer*	target
	)
{
	if (!isDrop)
	{
		itsLayout->SetSelectedWidgetsVisible(true);
	}
}
