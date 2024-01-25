/******************************************************************************
 WidgetSet.cpp

	BASE CLASS = ContainerWidget

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
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
}

WidgetSet::WidgetSet
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
	ContainerWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
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

	ContainerWidget::StreamOut(output);
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
