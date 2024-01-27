/******************************************************************************
 MenuBar.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "MenuBar.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuBar::MenuBar
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
	LayoutWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	MenuBarX(layout);
}

MenuBar::MenuBar
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
	MenuBarX(layout);
}

// private

void
MenuBar::MenuBarX
	(
	LayoutContainer* layout
	)
{
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MenuBar::~MenuBar()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
MenuBar::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("MenuBar") << std::endl;

	LayoutWidget::StreamOut(output);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
MenuBar::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.String(GetAperture(), "JXMenuBar", JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
MenuBar::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawUpFrame(p, frame, 2);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
MenuBar::GetClassName()
	const
{
	return "JXMenuBar";
}
