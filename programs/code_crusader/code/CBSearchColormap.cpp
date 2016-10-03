/******************************************************************************
 CBSearchColormap.cpp

	BASE CLASS = JColormap

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSearchColormap.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchColormap::CBSearchColormap()
	:
	JColormap()
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

JColorIndex
CBSearchColormap::GetBlackColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetRedColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetGreenColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetYellowColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetBlueColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetMagentaColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetCyanColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetWhiteColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetGrayColor
	(
	const JSize percentage
	)
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDarkRedColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetOrangeColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDarkGreenColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetLightBlueColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetBrownColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetPinkColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultSelectionColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultBackColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultFocusColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultSliderBackColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetInactiveLabelColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultSelButtonColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::GetDefaultDNDBorderColor()
	const
{
	return 0;
}

JColorIndex
CBSearchColormap::Get3DLightColor()
	const
{
	return 0;
}

JColorIndex
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
	JColorIndex*		colorIndex
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
	const JColorIndex	colorIndex,
	JSize*				red,
	JSize*				green,
	JSize*				blue
	)
	const
{
	*red = *green = *blue = 0;
}
