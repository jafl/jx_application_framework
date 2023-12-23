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

static const JUtf8Byte* kDNDClassID = "BaseWidget";

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
	output << GetEnclosure()->GlobalToLocal(GetFrameGlobal()) << std::endl;

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
		SetSelected(!IsSelected());
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

			auto* data = jnew LayoutSelection(itsLayout, kDNDClassID);
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
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
BaseWidget::GetSelectionData
	(
	JXSelectionData*	data,
	const JString&		id
	)
{
	if (id == kDNDClassID)
	{
		auto* layoutData = dynamic_cast<LayoutSelection*>(data);
		assert( layoutData != nullptr );

		JPtrArray<BaseWidget> widgetList(JPtrArrayT::kForgetAll);
		itsLayout->GetSelectedWidgets(&widgetList);
		assert( !widgetList.IsEmpty() );

		layoutData->SetData(widgetList);
	}
	else
	{
		JXWidget::GetSelectionData(data, id);
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
	if ((style == JXMenu::kMacintoshStyle && modifiers.meta()) ||
		(style == JXMenu::kWindowsStyle   && modifiers.control()))
	{
		return GetDNDManager()->GetDNDActionMoveXAtom();
	}
	else
	{
		return GetDNDManager()->GetDNDActionCopyXAtom();
	}
}
