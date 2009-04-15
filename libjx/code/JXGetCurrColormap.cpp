/******************************************************************************
 JXGetCurrColormap.cpp

	This class exists only because ParseFunction() needs to be able to
	create JUserInputFunctions without being passed a JColormap*

	We simply ask the application (which is global) for the current display,
	and then get the colormap from there.

	*** This forces all windows containing JXExprEditors to use the
		display's colormap instead of a custom one.

	BASE CLASS = JGetCurrentColormap

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXGetCurrColormap.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetCurrColormap::JXGetCurrColormap()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetCurrColormap::~JXGetCurrColormap()
{
}

/******************************************************************************
 GetCurrColormap

 ******************************************************************************/

JColormap*
JXGetCurrColormap::GetCurrColormap()
	const
{
	return ((JXGetApplication())->GetCurrentDisplay())->GetColormap();
}
