/******************************************************************************
 CBSearchColormap.cpp

	BASE CLASS = JXColorManager

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBSearchColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchColormap::CBSearchColormap()
	:
	JXColorManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchColormap::~CBSearchColormap()
{
}

/******************************************************************************
 Pre-allocated colors (virtual)

 ******************************************************************************/

JColorID
CBSearchColormap::GetBlackColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetRedColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetGreenColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetYellowColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetBlueColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetMagentaColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetCyanColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetWhiteColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetGrayColor
	(
	const JSize percentage
	)
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDarkRedColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetOrangeColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDarkGreenColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetLightBlueColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetBrownColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetPinkColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultSelectionColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultBackColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultFocusColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultSliderBackColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetInactiveLabelColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultSelButtonColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::GetDefaultDNDBorderColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::Get3DLightColor()
	const
{
	return 0;
}

JColorID
CBSearchColormap::Get3DShadeColor()
	const
{
	return 0;
}

/******************************************************************************
 GetColor (virtual)

 ******************************************************************************/

JBoolean
CBSearchColormap::GetColor
	(
	const JCharacter*	name,
	JColorID*		colorIndex
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
CBSearchColormap::GetColor
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
CBSearchColormap::GetRGB
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
