/******************************************************************************
 CoreWidget.cpp

	BASE CLASS = LayoutWidget

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
	const bool			wantsInput,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(wantsInput, layout, hSizing, vSizing, x,y, w,h)
{
}

CoreWidget::CoreWidget
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
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
