/******************************************************************************
 ScrollbarSet.cpp

	BASE CLASS = BaseWidget

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
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	ScrollbarSetX(layout);
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
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	ScrollbarSetX(layout);
}

// private

void
ScrollbarSet::ScrollbarSetX
	(
	LayoutContainer* layout
	)
{
	itsLayout = jnew LayoutContainer(layout, this, this, kHElastic, kVElastic, 0,0, 100,100);
	itsLayout->FitToEnclosure();
	itsLayout->AdjustSize(-kScrollbarWidth, -kScrollbarWidth);
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

	BaseWidget::StreamOut(output);
}

/******************************************************************************
 GetLayoutContainer (virtual)

	Some widgets can contain other widgets.

 ******************************************************************************/

bool
ScrollbarSet::GetLayoutContainer
	(
	LayoutContainer** layout
	)
	const
{
	*layout = itsLayout;
	return true;
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
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ScrollbarSet::DrawBorder
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
ScrollbarSet::GetClassName()
	const
{
	return "JXScrollbarSet";
}

/******************************************************************************
 GetEnclosureName (virtual protected)

 ******************************************************************************/

JString
ScrollbarSet::GetEnclosureName()
	const
{
	bool b;
	return GetVarName(&b) + "->GetScrollEnclosure()";
}

/******************************************************************************
 PrepareToGenerateCode (virtual)

 ******************************************************************************/

void
ScrollbarSet::PrepareToGenerateCode()
	const
{
	itsLayout->AdjustSize(kScrollbarWidth, kScrollbarWidth);
}

/******************************************************************************
 GenerateCodeFinished (virtual)

 ******************************************************************************/

void
ScrollbarSet::GenerateCodeFinished()
	const
{
	itsLayout->AdjustSize(-kScrollbarWidth, -kScrollbarWidth);
}
