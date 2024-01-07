/******************************************************************************
 TextCheckbox.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextCheckbox.h"
#include "WidgetLabelPanel.h"
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextCheckbox::TextCheckbox
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
	CoreWidget(layout, false, enclosure, hSizing, vSizing, x,y, w,h)
{
	TextCheckboxX(JGetString("DefaultLabel::TextCheckbox"), x,y,w,h);
}

TextCheckbox::TextCheckbox
	(
	LayoutContainer*	layout,
	const JString&		label,
	const JString&		shortcuts,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(layout, false, enclosure, hSizing, vSizing, x,y, w,h),
	itsShortcuts(shortcuts)
{
	TextCheckboxX(label, x,y,w,h);
}

TextCheckbox::TextCheckbox
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
	input >> label >> itsShortcuts;

	TextCheckboxX(label, x,y,w,h);
}

// private

void
TextCheckbox::TextCheckboxX
	(
	const JString&		label,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsCheckbox = jnew JXTextCheckbox(label, this, kHElastic, kVElastic, x,y,w,h);
	SetWidget(itsCheckbox);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextCheckbox::~TextCheckbox()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
TextCheckbox::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("TextCheckbox") << std::endl;

	CoreWidget::StreamOut(output);

	output << itsCheckbox->GetLabel() << std::endl;
	output << itsShortcuts << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
TextCheckbox::ToString()
	const
{
	JString s = CoreWidget::ToString();
	if (!itsShortcuts.IsEmpty())
	{
		s += JString::newline;
		s += JGetString("ShortcutsLabel::BaseWidget");
		s += itsShortcuts;
	}
	return s;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
TextCheckbox::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew WidgetLabelPanel(dlog, itsCheckbox->GetLabel(), itsShortcuts);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
TextCheckbox::SavePanelData()
{
	JString label;
	itsPanel->GetValues(&label, &itsShortcuts);
	itsCheckbox->SetLabel(label);

	itsPanel = nullptr;
}
