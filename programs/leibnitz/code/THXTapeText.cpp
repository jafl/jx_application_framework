/******************************************************************************
 THXTapeText.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXTapeText.h"
#include <jXConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

THXTapeText::THXTapeText
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor, kJFalse, kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	SetDefaultFontSize(JGetDefaultFontSize());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXTapeText::~THXTapeText()
{
}
