/******************************************************************************
 TextRadioButton.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextRadioButton.h"
#include "RadioButtonIDPanel.h"
#include "WidgetLabelPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kMarginWidth   = 5;
const JCoordinate kBoxHeight     = 12;
const JCoordinate kBoxHalfHeight = 6;

/******************************************************************************
 Constructor

 ******************************************************************************/

TextRadioButton::TextRadioButton
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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsLabel(JGetString("DefaultLabel::TextRadioButton"))
{
}

TextRadioButton::TextRadioButton
	(
	const JString&		id,
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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsID(id),
	itsLabel(label),
	itsShortcuts(shortcuts)
{
}

TextRadioButton::TextRadioButton
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
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	input >> itsID >> itsLabel >> itsShortcuts;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextRadioButton::~TextRadioButton()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
TextRadioButton::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("TextRadioButton") << std::endl;

	BaseWidget::StreamOut(output);

	output << itsID << std::endl;
	output << itsLabel << std::endl;
	output << itsShortcuts << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
TextRadioButton::ToString()
	const
{
	JString s = BaseWidget::ToString();

	s += JString::newline;
	s += JGetString("IDLabel::TextRadioButton");
	s += itsID;

	if (!itsShortcuts.IsEmpty())
	{
		s += JString::newline;
		s += JGetString("ShortcutsLabel::BaseWidget");
		s += itsShortcuts;
	}
	return s;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
TextRadioButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds  = GetBounds();
	const JCoordinate y = bounds.ycenter();

	// draw button

	const JRect boxRect(y - kBoxHalfHeight, kMarginWidth,
						y + kBoxHalfHeight, kMarginWidth + kBoxHeight);
	JXDrawUpDiamond(p, boxRect, kJXDefaultBorderWidth, true, JColorManager::GetDefaultBackColor());

	// draw text

	JRect textRect  = bounds;
	textRect.left  += 2*kMarginWidth + kBoxHeight;
	p.String(textRect, itsLabel, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
TextRadioButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
TextRadioButton::GetClassName()
	const
{
	return "JXTextRadioButton";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
TextRadioButton::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	itsID.Print(output);
	output << ", ";

	PrintStringForArg(itsLabel, varName, stringdb, output);
	output << ", ";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
TextRadioButton::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (!itsShortcuts.IsEmpty())
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetShortcuts(";
		PrintStringForArg(itsShortcuts, varName + "::shortcuts", stringdb, output);
		output << ");" << std::endl;
	}
	else
	{
		BaseWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
TextRadioButton::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsIDPanel    = jnew RadioButtonIDPanel(dlog, itsID);
	itsLabelPanel = jnew WidgetLabelPanel(dlog, itsLabel, itsShortcuts);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
TextRadioButton::SavePanelData()
{
	itsIDPanel->GetValues(&itsID);
	itsLabelPanel->GetValues(&itsLabel, &itsShortcuts);

	itsIDPanel    = nullptr;
	itsLabelPanel = nullptr;
}
