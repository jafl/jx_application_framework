/******************************************************************************
 ScrollbarSet.cpp

	BASE CLASS = ContainerWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ScrollbarSet.h"
#include "LayoutContainer.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kScrollbarWidth = 15;

/******************************************************************************
 Constructor

 ******************************************************************************/

ScrollbarSet::ScrollbarSet
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
	ContainerWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	ScrollbarSetX();
}

ScrollbarSet::ScrollbarSet
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
	ScrollbarSetX();
}

// private

void
ScrollbarSet::ScrollbarSetX()
{
	GetLayoutContainer()->AdjustSize(-kScrollbarWidth, -kScrollbarWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ScrollbarSet::~ScrollbarSet()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ScrollbarSet::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ScrollbarSet") << std::endl;

	ContainerWidget::StreamOut(output);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ScrollbarSet::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor(JColorManager::GetDefaultSliderBackColor());
	p.SetFilling(true);
	p.Rect(GetAperture());
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ScrollbarSet::GetClassName()
	const
{
	return "JXScrollbarSet";
}

/******************************************************************************
 GetEnclosureName (virtual protected)

 ******************************************************************************/

JString
ScrollbarSet::GetEnclosureName
	(
	const LayoutWidget* widget
	)
	const
{
	bool b1, b2;
	return GetVarName(&b1, &b2) + "->GetScrollEnclosure()";
}

/******************************************************************************
 PrepareToGenerateCode (virtual)

 ******************************************************************************/

void
ScrollbarSet::PrepareToGenerateCode
	(
	std::ostream&	output,
	const JString&	indent,
	JStringManager*	stringdb
	)
	const
{
	GetLayoutContainer()->AdjustSize(kScrollbarWidth, kScrollbarWidth);
}

/******************************************************************************
 GenerateCodeFinished (virtual)

 ******************************************************************************/

void
ScrollbarSet::GenerateCodeFinished
	(
	std::ostream&	output,
	const JString&	indent,
	JStringManager*	stringdb
	)
	const
{
	GetLayoutContainer()->AdjustSize(-kScrollbarWidth, -kScrollbarWidth);
}
