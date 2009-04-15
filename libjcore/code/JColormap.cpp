/******************************************************************************
 JColormap.cpp

	Pure virtual base class for system dependent colormap.

	BASE CLASS = none

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JColormap.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JColormap::JColormap()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JColormap::~JColormap()
{
}

/******************************************************************************
 SetDynamicColors (virtual)

	Sets the rgb components for the specified colors.  This is provided to
	allow optimization when possible.  The default implementation is to
	call SetDynamicColor().

 ******************************************************************************/

void
JColormap::SetDynamicColors
	(
	const JArray<JDynamicColorInfo>& colorList
	)
{
	const JSize count = colorList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDynamicColorInfo info = colorList.GetElement(i);
		SetDynamicColor(info.index, info.color);
		}
}
