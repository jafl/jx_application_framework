/******************************************************************************
 TEColormap.cpp

	BASE CLASS = JColormap

	Written by John Lindal.

 ******************************************************************************/

#include "TEColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TEColormap::TEColormap()
	:
	JColormap()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TEColormap::~TEColormap()
{
}

/******************************************************************************
 Pre-allocated colors (virtual)

 ******************************************************************************/

JColorIndex
TEColormap::GetBlackColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetRedColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetGreenColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetYellowColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetBlueColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetMagentaColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetCyanColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetWhiteColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetGrayColor
	(
	const JSize percentage
	)
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDarkRedColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetOrangeColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDarkGreenColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetLightBlueColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetBrownColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetPinkColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultSelectionColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultBackColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultFocusColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultSliderBackColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetInactiveLabelColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultSelButtonColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::GetDefaultDNDBorderColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::Get3DLightColor()
	const
{
	return 0;
}

JColorIndex
TEColormap::Get3DShadeColor()
	const
{
	return 0;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JBoolean
TEColormap::GetColor
	(
	const JString&	name,
	JColorIndex*	colorIndex
	)
	const
{
	*colorIndex = 0;
	return kJTrue;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JColorIndex
TEColormap::GetColor
	(
	const JSize	red,
	const JSize	green,
	const JSize	blue
	)
	const
{
	return 0;
}

/******************************************************************************
 GetRGB (virtual)

 ******************************************************************************/

void
TEColormap::GetRGB
	(
	const JColorIndex	colorIndex,
	JSize*				red,
	JSize*				green,
	JSize*				blue
	)
	const
{
	*red = *green = *blue = 0;
}
