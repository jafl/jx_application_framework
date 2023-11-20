/******************************************************************************
 BaseWidget.cpp

	BASE CLASS = JXWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "BaseWidget.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

BaseWidget::BaseWidget
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
}

BaseWidget::BaseWidget
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	input >> itsVarName >> itsMemberVarFlag;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BaseWidget::~BaseWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
BaseWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	// read by LayoutDirector

	output << (int) GetHSizing() << std::endl;
	output << (int) GetVSizing() << std::endl;
	output << GetEnclosure()->GlobalToLocal(GetFrameGlobal()) << std::endl;

	// read by ctor

	output << itsVarName << std::endl;
	output << itsMemberVarFlag << std::endl;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
BaseWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}
