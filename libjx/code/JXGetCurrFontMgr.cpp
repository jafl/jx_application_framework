/******************************************************************************
 JXGetCurrFontMgr.cpp

	This class exists only because ParseFunction() needs to be able to
	create JUserInputFunctions without being passed a JFontManager*

	We simply ask the application (which is global) for the current display,
	and then get the font manager from there.

	BASE CLASS = JGetCurrentFontManager

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXGetCurrFontMgr.h>
#include <jXGlobals.h>
#include <JXDisplay.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetCurrFontMgr::JXGetCurrFontMgr()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetCurrFontMgr::~JXGetCurrFontMgr()
{
}

/******************************************************************************
 GetCurrFontManager

 ******************************************************************************/

const JFontManager*
JXGetCurrFontMgr::GetCurrFontManager()
	const
{
	return ((JXGetApplication())->GetCurrentDisplay())->GetFontManager();
}
