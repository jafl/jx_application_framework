/******************************************************************************
 JXWidgetSet.cpp

	Maintains a set of JXWidgets that are required to act as a unit.

	BASE CLASS = JXWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXWidgetSet.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWidgetSet::JXWidgetSet
	(
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
	itsNeedsInternalFTCFlag(kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWidgetSet::~JXWidgetSet()
{
}

/******************************************************************************
 Draw (virtual protected)

	There is nothing to do since we only act as a container.

 ******************************************************************************/

void
JXWidgetSet::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

	There is nothing to do since we only act as a container.

 ******************************************************************************/

void
JXWidgetSet::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 NeedsInternalFTC (virtual protected)

	Return kJTrue if the contents are a set of widgets that need to expand.

 ******************************************************************************/

JBoolean
JXWidgetSet::NeedsInternalFTC()
	const
{
	return itsNeedsInternalFTCFlag;
}
