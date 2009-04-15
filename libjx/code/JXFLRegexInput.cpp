/******************************************************************************
 JXFLRegexInput.cpp

	Input field for entering a file regex.

	BASE CLASS = JXFLInputBase

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFLRegexInput.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFLRegexInput::JXFLRegexInput
	(
	JXFileListSet*			flSet,
	JXStringHistoryMenu*	historyMenu,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXFLInputBase(flSet, historyMenu, enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFLRegexInput::~JXFLRegexInput()
{
}

/******************************************************************************
 GetRegexString (virtual)

 ******************************************************************************/

JString
JXFLRegexInput::GetRegexString()
	const
{
	return GetText();
}
