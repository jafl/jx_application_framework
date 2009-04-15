/******************************************************************************
 JXFLWildcardInput.cpp

	Input field for entering a wildcard filter.  (e.g. "*.cc")

	BASE CLASS = JXFLInputBase

	Copyright © 1998 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFLWildcardInput.h>
#include <JDirInfo.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFLWildcardInput::JXFLWildcardInput
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

JXFLWildcardInput::~JXFLWildcardInput()
{
}

/******************************************************************************
 GetRegexString (virtual)

 ******************************************************************************/

JString
JXFLWildcardInput::GetRegexString()
	const
{
	JString regexStr;
	JDirInfo::BuildRegexFromWildcardFilter(GetText(), &regexStr);
	return regexStr;
}
