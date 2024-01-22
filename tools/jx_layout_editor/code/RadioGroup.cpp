/******************************************************************************
 RadioGroup.cpp

	BASE CLASS = BaseWidget

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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	RadioGroupX(layout);
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
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	RadioGroupX(layout);
}

// private

void
RadioGroup::RadioGroupX
	(
	LayoutContainer* layout
	)
{
	SetBorderWidth(2);

	itsLayout = jnew LayoutContainer(layout, this, this, kHElastic, kVElastic, 0,0, 100,100);
	itsLayout->FitToEnclosure();
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

	BaseWidget::StreamOut(output);
}

/******************************************************************************
 GetLayoutContainer (virtual)

	Some widgets can contain other widgets.

 ******************************************************************************/

bool
RadioGroup::GetLayoutContainer
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
RadioGroup::Draw
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
RadioGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEngravedFrame(p, frame, 1, 0, 1);
}

/******************************************************************************
 DrawOver (virtual protected)

 ******************************************************************************/

void
RadioGroup::DrawOver
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	itsLayout->SetHint(ToString());
	BaseWidget::DrawOver(p, rect);
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
