/******************************************************************************
 ComplexBorderRect.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ComplexBorderRect.h"
#include "ComplexBorderWidthPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ComplexBorderRect::ComplexBorderRect
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
	itsOutsideWidth(1),
	itsBetweenWidth(0),
	itsInsideWidth(1)
{
	ComplexBorderRectX();
}

ComplexBorderRect::ComplexBorderRect
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
	input >> type >> itsOutsideWidth >> itsBetweenWidth >> itsInsideWidth;

	itsType = (Type) type;

	ComplexBorderRectX();
}

// private

void
ComplexBorderRect::ComplexBorderRectX()
{
	SetBorderWidth(itsOutsideWidth + itsBetweenWidth + itsInsideWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ComplexBorderRect::~ComplexBorderRect()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ComplexBorderRect::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ComplexBorderRect") << std::endl;

	ContainerWidget::StreamOut(output);

	output << (int) itsType << std::endl;
	output << itsOutsideWidth << std::endl;
	output << itsBetweenWidth << std::endl;
	output << itsInsideWidth << std::endl;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ComplexBorderRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (itsType == kEmbossedType)
	{
		JXDrawEmbossedFrame(p, frame, itsOutsideWidth, itsBetweenWidth, itsInsideWidth);
	}
	else
	{
		JXDrawEngravedFrame(p, frame, itsOutsideWidth, itsBetweenWidth, itsInsideWidth);
	}
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ComplexBorderRect::GetClassName()
	const
{
	return itsType == kEmbossedType ? "JXEmbossedRect" : "JXEngravedRect";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
ComplexBorderRect::PrintConfiguration
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
	output << "->SetWidths(" << itsOutsideWidth << ", ";
	output << itsBetweenWidth << ", " << itsInsideWidth << ");" << std::endl;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
ComplexBorderRect::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew ComplexBorderWidthPanel(dlog, itsOutsideWidth, itsBetweenWidth, itsInsideWidth);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
ComplexBorderRect::SavePanelData()
{
	itsPanel->GetValues(&itsOutsideWidth, &itsBetweenWidth, &itsInsideWidth);
	itsPanel = nullptr;

	ComplexBorderRectX();
}
