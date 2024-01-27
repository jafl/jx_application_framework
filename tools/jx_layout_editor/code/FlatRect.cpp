/******************************************************************************
 FlatRect.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "FlatRect.h"
#include "ColorChooserPanel.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FlatRect::FlatRect
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
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsColor(JColorManager::GetDefaultBackColor())
{
}

FlatRect::FlatRect
	(
	const JColorID		color,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsColor(color)
{
}

FlatRect::FlatRect
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
	ContainerWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	JRGB color;
	input >> color;
	itsColor = JColorManager::GetColorID(color);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FlatRect::~FlatRect()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
FlatRect::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("FlatRect") << std::endl;

	ContainerWidget::StreamOut(output);

	output << JColorManager::GetRGB(itsColor) << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
FlatRect::ToString()
	const
{
	JString s = ContainerWidget::ToString();

	std::ostringstream c;
	c << std::hex << JColorManager::GetRGB(itsColor);

	s += JString::newline;
	s += JGetString("ColorLabel::FlatRect");
	s += c.str();

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
FlatRect::GetClassName()
	const
{
	return "JXFlatRect";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
FlatRect::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	indent.Print(output);
	varName.Print(output);
	output << "->SetBackColor(";
	ColorChooserPanel::PrintColorConfiguration(itsColor, output);
	output << ");" << std::endl;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
FlatRect::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew ColorChooserPanel(dlog, itsColor);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
FlatRect::SavePanelData()
{
	itsPanel->GetValues(&itsColor);
	itsPanel = nullptr;
}
