/******************************************************************************
 BaseWidget.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "BaseWidget.h"
#include "LayoutDirector.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

BaseWidget::BaseWidget
	(
	LayoutDirector*		dir,
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
	itsLayoutDir(dir),
	itsMemberVarFlag(false),
	itsSelectedFlag(false)
{
}

BaseWidget::BaseWidget
	(
	LayoutDirector*		dir,
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
	itsLayoutDir(dir),
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
	// read by LayoutDirector

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
	if (button == kJXLeftButton && clickCount == 1)
	{
		GetLayoutDirector()->ClearSelection();
		SetSelected(true);
	}
	else if (button == kJXLeftButton && clickCount == 2)
	{
		// TODO:  edit parameters
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
}
