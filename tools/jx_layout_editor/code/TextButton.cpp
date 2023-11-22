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
	CoreWidget(dir, enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(JGetString("DefaultLabel::TextButton"), x,y,w,h);
}

TextButton::TextButton
	(
	LayoutDirector*		dir,
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
	CoreWidget(dir, enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(label, x,y,w,h);
}

TextButton::TextButton
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
	CoreWidget(dir, input, enclosure, hSizing, vSizing, x,y, w,h)
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
	itsButton = jnew JXTextButton(label, this, kFixedLeft, kFixedTop, x,y,w,h);
	SetWidget(itsButton);
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

	output << itsButton->GetLabel() << std::endl;
}

/******************************************************************************
 Label

 ******************************************************************************/

const JString&
TextButton::GetLabel()
	const
{
	return itsButton->GetLabel();
}

void
TextButton::SetLabel
	(
	const JString& label
	)
{
	itsButton->SetLabel(label);
}
