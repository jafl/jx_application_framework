/******************************************************************************
 TextCheckbox.cpp

	BASE CLASS = TextButtonBase

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
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	TextButtonBase(layout, hSizing, vSizing, x,y, w,h)
{
	TextCheckboxX(JGetString("DefaultLabel::TextCheckbox"), x,y,w,h);
}

TextCheckbox::TextCheckbox
	(
	const JString&		label,
	const JString&		shortcuts,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	TextButtonBase(layout, hSizing, vSizing, x,y, w,h),
	itsShortcuts(shortcuts)
{
	TextCheckboxX(label, x,y,w,h);
}

TextCheckbox::TextCheckbox
	(
	std::istream&		input,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	TextButtonBase(input, layout, hSizing, vSizing, x,y, w,h)
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

	TextButtonBase::StreamOut(output);

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
	JString s = TextButtonBase::ToString();
	if (!itsShortcuts.IsEmpty())
	{
		s += JString::newline;
		s += JGetString("ShortcutsLabel::BaseWidget");
		s += itsShortcuts;
	}
	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
TextCheckbox::GetClassName()
	const
{
	return "JXTextCheckbox";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
TextCheckbox::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	SharedPrintCtorArgsWithComma(output, varName, itsCheckbox->GetLabel(), stringdb);
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
TextCheckbox::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (!SharedPrintConfiguration(output, indent, varName, itsShortcuts, stringdb))
	{
		TextButtonBase::PrintConfiguration(output, indent, varName, stringdb);
	}
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
