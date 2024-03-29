/******************************************************************************
 InputFieldBase.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "InputFieldBase.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

InputFieldBase::InputFieldBase
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
	LayoutWidget(true, layout, hSizing, vSizing, x,y, w,h)
{
	InputFieldBaseX();
}

InputFieldBase::InputFieldBase
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
	InputFieldBaseX();
}

// private

void
InputFieldBase::InputFieldBaseX()
{
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

InputFieldBase::~InputFieldBase()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
InputFieldBase::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor(JColorManager::GetWhiteColor());
	p.SetFilling(true);
	p.Rect(GetBounds());
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
InputFieldBase::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, 2);
}
