/******************************************************************************
 ProgressIndicator.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ProgressIndicator.h"
#include <jx-af/jx/JXProgressIndicator.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ProgressIndicator::ProgressIndicator
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
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
	ProgressIndicatorX(x,y,w,h);
}

ProgressIndicator::ProgressIndicator
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
	ProgressIndicatorX(x,y,w,h);
}

// private

void
ProgressIndicator::ProgressIndicatorX
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsWidget = jnew JXProgressIndicator(this, kHElastic, kVElastic, x,y,w,h);
	itsWidget->SetMaxValue(10);
	itsWidget->SetValue(4);
	SetWidget(itsWidget);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ProgressIndicator::~ProgressIndicator()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ProgressIndicator::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ProgressIndicator") << std::endl;

	CoreWidget::StreamOut(output);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ProgressIndicator::GetClassName()
	const
{
	return "JXProgressIndicator";
}
