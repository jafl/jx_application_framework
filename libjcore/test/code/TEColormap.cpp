/******************************************************************************
 TEColormap.cpp

	BASE CLASS = JColorManager

	Written by John Lindal.

 ******************************************************************************/

#include "TEColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TEColormap::TEColormap()
	:
	JColorManager()
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

JColorID
TEColormap::GetBlackColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetRedColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetGreenColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetYellowColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetBlueColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetMagentaColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetCyanColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetWhiteColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetGrayColor
	(
	const JSize percentage
	)
	const
{
	return 0;
}

JColorID
TEColormap::GetDarkRedColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetOrangeColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDarkGreenColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetLightBlueColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetBrownColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetPinkColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultSelectionColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultBackColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultFocusColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultSliderBackColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetInactiveLabelColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultSelButtonColor()
	const
{
	return 0;
}

JColorID
TEColormap::GetDefaultDNDBorderColor()
	const
{
	return 0;
}

JColorID
TEColormap::Get3DLightColor()
	const
{
	return 0;
}

JColorID
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
	JColorID*	colorIndex
	)
	const
{
	*colorIndex = 0;
	return kJTrue;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JColorID
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
	const JColorID	colorIndex,
	JSize*				red,
	JSize*				green,
	JSize*				blue
	)
	const
{
	*red = *green = *blue = 0;
}
