/******************************************************************************
 SimpleBorderRect.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "SimpleBorderRect.h"
#include "BorderWidthPanel.h"
#include "ColorChooserPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SimpleBorderRect::SimpleBorderRect
	(
	const Type			type,
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
	itsType(type),
	itsWidth(2),
	itsColor(JColorManager::GetBlackColor())
{
	SimpleBorderRectX();
}

SimpleBorderRect::SimpleBorderRect
	(
	const Type			type,
	const JSize			width,
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
	itsType(type),
	itsWidth(width),
	itsColor(color)
{
	SimpleBorderRectX();
}

SimpleBorderRect::SimpleBorderRect
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
	int type;
	JRGB rgb;
	input >> type >> itsWidth >> rgb;

	itsType  = (Type) type;
	itsColor = JColorManager::GetColorID(rgb);

	SimpleBorderRectX();
}

// private

void
SimpleBorderRect::SimpleBorderRectX()
{
	SetBorderWidth(itsWidth);
	if (itsType == kBorderType)
	{
		SetBackColor(itsColor);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SimpleBorderRect::~SimpleBorderRect()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
SimpleBorderRect::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("SimpleBorderRect") << std::endl;

	ContainerWidget::StreamOut(output);

	output << (int) itsType << std::endl;
	output << itsWidth << std::endl;
	output << JColorManager::GetRGB(itsColor) << std::endl;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
SimpleBorderRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (itsType == kUpType)
	{
		JXDrawUpFrame(p, frame, itsWidth);
	}
	else if (itsType == kDownType)
	{
		JXDrawDownFrame(p, frame, itsWidth);
	}
	else
	{
		p.SetPenColor(itsColor);
		p.SetFilling(true);
		p.Rect(frame);
	}
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
SimpleBorderRect::GetClassName()
	const
{
	return (itsType == kUpType   ? "JXUpRect" :
			itsType == kDownType ? "JXDownRect" :
			"JXBorderRect");
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
SimpleBorderRect::PrintConfiguration
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
	output << "->SetBorderWidth(" << itsWidth << ");" << std::endl;

	if (itsType == kBorderType)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetBackColor(";
		ColorChooserPanel::PrintColorConfiguration(itsColor, output);
		output << ");" << std::endl;
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
SimpleBorderRect::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsWidthPanel = jnew BorderWidthPanel(dlog, itsWidth);

	if (itsType == kBorderType)
	{
		itsColorPanel = jnew ColorChooserPanel(dlog, itsColor);
	}
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
SimpleBorderRect::SavePanelData()
{
	itsWidthPanel->GetValues(&itsWidth);
	itsWidthPanel = nullptr;

	if (itsType == kBorderType)
	{
		itsColorPanel->GetValues(&itsColor);
		itsColorPanel = nullptr;
	}

	SimpleBorderRectX();
}
