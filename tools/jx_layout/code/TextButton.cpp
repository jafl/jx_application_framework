/******************************************************************************
 TextButton.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextButton.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextButton::TextButton
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
	BaseWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(x,y,w,h);
}

TextButton::TextButton
	(
	const JString&		label,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsLabel(label)
{
	TextButtonX(x,y,w,h);
}

TextButton::TextButton
	(
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
	BaseWidget(input, enclosure, hSizing, vSizing, x,y, w,h)
{
	input >> itsLabel;

	TextButtonX(x,y,w,h);
}

// private

void
TextButton::TextButtonX
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsWidget = jnew JXTextButton(itsLabel, this, kFixedLeft, kFixedTop, x,y,w,h);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextButton::~TextButton()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
TextButton::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("TextButton", JString::kNoCopy) << std::endl;

	BaseWidget::StreamOut(output);

	output << itsLabel << std::endl;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
TextButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	itsWidget->Place(rect.left, rect.top);
	itsWidget->SetSize(rect.width(), rect.height());
}

/******************************************************************************
 SetLabel

 ******************************************************************************/

void
TextButton::SetLabel
	(
	const JString& label
	)
{
	itsLabel = label;
	itsWidget->SetLabel(label);
}
