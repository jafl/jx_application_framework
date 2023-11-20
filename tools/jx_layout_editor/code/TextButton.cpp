/******************************************************************************
 TextButton.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextButton.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jGlobals.h>
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
	CoreWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(JGetString("DefaultLabel::TextButton"), x,y,w,h);
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
	CoreWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(label, x,y,w,h);
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
	CoreWidget(input, enclosure, hSizing, vSizing, x,y, w,h)
{
	JString label;
	input >> label;

	TextButtonX(label, x,y,w,h);
}

// private

void
TextButton::TextButtonX
	(
	const JString&		label,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsWidget = jnew JXTextButton(label, this, kFixedLeft, kFixedTop, x,y,w,h);
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

	output << itsWidget->GetLabel() << std::endl;
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
 Label

 ******************************************************************************/

const JString&
TextButton::GetLabel()
	const
{
	return itsWidget->GetLabel();
}

void
TextButton::SetLabel
	(
	const JString& label
	)
{
	itsWidget->SetLabel(label);
}
