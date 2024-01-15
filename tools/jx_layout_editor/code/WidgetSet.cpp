/******************************************************************************
 WidgetSet.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetSet.h"
#include "LayoutContainer.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WidgetSet::WidgetSet
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	WidgetSetX(layout);
}

WidgetSet::WidgetSet
	(
	std::istream&		input,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(input, layout, hSizing, vSizing, x,y, w,h)
{
	WidgetSetX(layout);
}

// private

void
WidgetSet::WidgetSetX
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

WidgetSet::~WidgetSet()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
WidgetSet::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("WidgetSet") << std::endl;

	BaseWidget::StreamOut(output);
}

/******************************************************************************
 GetLayoutContainer (virtual)

	Some widgets can contain other widgets.

 ******************************************************************************/

bool
WidgetSet::GetLayoutContainer
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
WidgetSet::Draw
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
WidgetSet::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
WidgetSet::GetClassName()
	const
{
	return "JXWidgetSet";
}
