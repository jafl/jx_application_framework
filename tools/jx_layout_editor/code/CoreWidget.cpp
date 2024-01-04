/******************************************************************************
 CoreWidget.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "CoreWidget.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CoreWidget::CoreWidget
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
	BaseWidget(layout, enclosure, hSizing, vSizing, x,y, w,h)
{
}

CoreWidget::CoreWidget
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
	BaseWidget(layout, input, enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CoreWidget::~CoreWidget()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CoreWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	DrawSelection(p, rect);

	itsWidget->Place(rect.left, rect.top);
	itsWidget->SetSize(rect.width(), rect.height());
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
CoreWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawOver (virtual protected)

 ******************************************************************************/

void
CoreWidget::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	DrawSelection(p, GetFrameLocal());
	BaseWidget::DrawOver(p, rect);
}

/******************************************************************************
 StealMouse (virtual protected)

	Don't pass mouse clicks to rendered widget.

 ******************************************************************************/

bool
CoreWidget::StealMouse
	(
	const int			eventType,
	const JPoint&		ptG,
	const JXMouseButton	button,
	const unsigned int	state
	)
{
	return true;
}
