/******************************************************************************
 TextButton.cpp

	BASE CLASS = TextButtonBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextButton.h"
#include "WidgetLabelPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextButton::TextButton
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
	TextButtonX(JGetString("DefaultLabel::TextButton"), x,y,w,h);
}

TextButton::TextButton
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
	TextButtonX(label, x,y,w,h);
}

TextButton::TextButton
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
	TextButtonBase(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	JString label;
	input >> label >> itsShortcuts;

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

	TextButtonBase::StreamOut(output);

	output << itsButton->GetLabel() << std::endl;
	output << itsShortcuts << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
TextButton::ToString()
	const
{
	JString s = TextButtonBase::ToString();
	if (!itsShortcuts.IsEmpty())
	{
		s += JString::newline;
		s += JGetString("ShortcutsLabel::LayoutWidget");
		s += itsShortcuts;
	}
	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
TextButton::GetClassName()
	const
{
	return "JXTextButton";
}

/******************************************************************************
 GetFrameForCode (virtual protected)

 ******************************************************************************/

JRect
TextButton::GetFrameForCode()
	const
{
	JRect r = GetFrame();
	if (itsShortcuts.Contains("^M", JString::kIgnoreCase))
	{
		r.Expand(1,1);
	}
	return r;
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
TextButton::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	PrintStringForArg(itsButton->GetLabel(), varName, stringdb, output);
	output << ", ";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
TextButton::PrintConfiguration
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
TextButton::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew WidgetLabelPanel(dlog, itsButton->GetLabel(), itsShortcuts);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
TextButton::SavePanelData()
{
	JString label;
	itsPanel->GetValues(&label, &itsShortcuts);
	itsButton->SetLabel(label);

	itsPanel = nullptr;
}
