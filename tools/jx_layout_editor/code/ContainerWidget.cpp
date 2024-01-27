/******************************************************************************
 ContainerWidget.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ContainerWidget.h"
#include "LayoutContainer.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ContainerWidget::ContainerWidget
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
	ContainerWidgetX(layout);
}

ContainerWidget::ContainerWidget
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
	ContainerWidgetX(layout);
}

// private

void
ContainerWidget::ContainerWidgetX
	(
	LayoutContainer* layout
	)
{
	itsLayout = jnew LayoutContainer(layout, this, this, kHElastic, kVElastic, 0,0, 100,100);
	itsLayout->FitToEnclosure();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ContainerWidget::~ContainerWidget()
{
}

/******************************************************************************
 GetLayoutContainer (virtual)

	Some widgets can contain other widgets.

 ******************************************************************************/

bool
ContainerWidget::GetLayoutContainer
	(
	LayoutContainer** layout
	)
	const
{
	*layout = itsLayout;
	return true;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ContainerWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ContainerWidget::DrawBorder
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
ContainerWidget::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	itsLayout->SetHint(ToString());
	LayoutWidget::DrawOver(p, rect);
}
