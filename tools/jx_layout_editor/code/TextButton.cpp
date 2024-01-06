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
	LayoutContainer*	layout,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(layout, enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(JGetString("DefaultLabel::TextButton"), x,y,w,h);
}

TextButton::TextButton
	(
	LayoutContainer*	layout,
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
	CoreWidget(layout, enclosure, hSizing, vSizing, x,y, w,h)
{
	TextButtonX(label, x,y,w,h);
}

TextButton::TextButton
	(
	LayoutContainer*	layout,
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
	CoreWidget(layout, input, enclosure, hSizing, vSizing, x,y, w,h)
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
	itsButton = jnew JXTextButton(label, this, kHElastic, kVElastic, x,y,w,h);
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
	output << JString("TextButton") << std::endl;

	BaseWidget::StreamOut(output);

	output << itsButton->GetLabel() << std::endl;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
TextButton::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
//	itsPanel = jnew WidgetLabelPanel(dlog, itsClassName, itsCtorArgs, itsCreateFlag));
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
TextButton::SavePanelData()
{
	// todo: extract values

//	itsPanel = nullptr;
}
