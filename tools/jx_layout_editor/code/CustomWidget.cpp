/******************************************************************************
 CustomWidget.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "CustomWidget.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CustomWidget::CustomWidget
	(
	LayoutDirector*		dir,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(dir, enclosure, hSizing, vSizing, x,y, w,h),
	itsCreateFlag(false)
{
	CustomWidgetX();
}

CustomWidget::CustomWidget
	(
	LayoutDirector*		dir,
	const JString&		className,
	const JString&		args,
	const bool			create,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(dir, enclosure, hSizing, vSizing, x,y, w,h),
	itsClassName(className),
	itsCtorArgs(args),
	itsCreateFlag(create)
{
	CustomWidgetX();
}

CustomWidget::CustomWidget
	(
	LayoutDirector*		dir,
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
	BaseWidget(dir, input, enclosure, hSizing, vSizing, x,y, w,h)
{
	input >> itsClassName >> itsCtorArgs >> itsCreateFlag;

	CustomWidgetX();
}

// private

void
CustomWidget::CustomWidgetX()
{
	SetBorderWidth(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CustomWidget::~CustomWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CustomWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("CustomWidget", JString::kNoCopy) << std::endl;

	BaseWidget::StreamOut(output);

	output << itsClassName << std::endl;
	output << itsCtorArgs << std::endl;
	output << itsCreateFlag << std::endl;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CustomWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.String(rect, itsClassName, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
CustomWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.Rect(frame);
	DrawSelection(p, frame);
}
