/******************************************************************************
 NewDirButton.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "NewDirButton.h"
#include <jx-af/jx/JXNewDirButton.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

NewDirButton::NewDirButton
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
	NewDirButtonX(x,y,w,h);
}

NewDirButton::NewDirButton
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
	NewDirButtonX(x,y,w,h);
}

// private

void
NewDirButton::NewDirButtonX
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsButton = jnew JXNewDirButton(this, kHElastic, kVElastic, x,y,w,h);
	SetWidget(itsButton);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

NewDirButton::~NewDirButton()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
NewDirButton::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("NewDirButton") << std::endl;

	CoreWidget::StreamOut(output);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
NewDirButton::GetClassName()
	const
{
	return "JXNewDirButton";
}
