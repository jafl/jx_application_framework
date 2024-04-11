/******************************************************************************
 Slider.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "Slider.h"
#include <jx-af/jx/JXSlider.h>
#include <jx-af/jx/JXLevelControl.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Slider::Slider
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
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsType(type)
{
	SliderX(x,y,w,h);
}

Slider::Slider
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
	CoreWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	int type;
	input >> type;

	itsType = (Type) type;

	SliderX(x,y,w,h);
}

// private

void
Slider::SliderX
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	if (itsType == kLevelControlType)
	{
		itsWidget = jnew JXLevelControl(this, kHElastic, kVElastic, x,y,w,h);
	}
	else
	{
		itsWidget = jnew JXSlider(this, kHElastic, kVElastic, x,y,w,h);
	}
	SetWidget(itsWidget);

	itsWidget->SetMaxValue(10);
	itsWidget->SetValue(4);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Slider::~Slider()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
Slider::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("Slider") << std::endl;

	CoreWidget::StreamOut(output);

	output << (int) itsType << std::endl;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
Slider::GetClassName()
	const
{
	return itsType == kLevelControlType ? "JXLevelControl" : "JXSlider";
}
