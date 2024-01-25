/******************************************************************************
 RadioGroup.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "RadioGroup.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RadioGroup::RadioGroup
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
	RadioGroupX();
}

RadioGroup::RadioGroup
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
	RadioGroupX();
}

// private

void
RadioGroup::RadioGroupX()
{
	SetBorderWidth(2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RadioGroup::~RadioGroup()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
RadioGroup::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("RadioGroup") << std::endl;

	ContainerWidget::StreamOut(output);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
RadioGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEngravedFrame(p, frame, 1, 0, 1);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
RadioGroup::GetClassName()
	const
{
	return "JXRadioGroup";
}
