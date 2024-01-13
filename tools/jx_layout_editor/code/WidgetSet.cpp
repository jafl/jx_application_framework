/******************************************************************************
 WidgetSet.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetSet.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WidgetSet::WidgetSet
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
	BaseWidget(layout, false, enclosure, hSizing, vSizing, x,y, w,h)
{
}

WidgetSet::WidgetSet
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
