/******************************************************************************
 JColorManager.cpp

	Abstract base class for system dependent color manager.

	BASE CLASS = none

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JColorManager.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JColorManager::JColorManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JColorManager::~JColorManager()
{
}

/******************************************************************************
 GetColor (static)

	We pack the values into a single 16 byte value.

 ******************************************************************************/

JColorID
JColorManager::GetColorID
	(
	const JRGB& rgb
	)
{
	return ((rgb.red << 32) | (rgb.green << 16) | rgb.blue);
}

/******************************************************************************
 GetRGB (static)

 ******************************************************************************/

JRGB
JColorManager::GetRGB
	(
	const JColorID colorID
	)
{
	JRGB rgb;
	rgb.red   = (colorID >> 32) & kJMaxRGBValue;
	rgb.green = (colorID >> 16) & kJMaxRGBValue;
	rgb.blue  = colorID & kJMaxRGBValue;
	return rgb;
}

/******************************************************************************
 Useful colors (static)

 ******************************************************************************/

enum
{
	kBlackColor,
	kRedColor,
	kGreenColor,
	kYellowColor,
	kBlueColor,
	kMagentaColor,
	kCyanColor,
	kWhiteColor,

	kDarkRedColor,
	kOrangeColor,
	kDarkGreenColor,
	kLightBlueColor,
	kBrownColor,
	kPinkColor,

	kDefaultSelectionColor
};

static const JRGB kDefColor[] =
{
	JRGB(     0,      0,      0),		// black
	JRGB(0xFFFF,      0,      0),		// red
	JRGB(     0, 0xFFFF,      0),		// green
	JRGB(0xFFFF, 0xFFFF,      0),		// yellow
	JRGB(     0,      0, 0xFFFF),		// blue
	JRGB(0xFFFF,      0, 0xFFFF),		// magenta
	JRGB(     0, 0xFFFF, 0xFFFF),		// cyan
	JRGB(0xFFFF, 0xFFFF, 0xFFFF),		// white

	JRGB(0x9999,      0,      0),		// dark red
	JRGB(0xFFFF, 0x9999,      0),		// orange
	JRGB(     0, 0x6666,      0),		// dark green
	JRGB(     0, 0x9999, 0xFFFF),		// light blue
	JRGB(0x9999, 0x6666,      0),		// brown
	JRGB(0xFFFF,      0, 0x9999),		// pink

	JRGB(0x9999, 0xFFFF, 0xFFFF)		// selection hilighting
};

JColorID
JColorManager::GetBlackColor()
{
	return GetColorID(kDefColor[ kBlackColor ]);
}

JColorID
JColorManager::GetRedColor()
{
	return GetColorID(kDefColor[ kRedColor ]);
}

JColorID
JColorManager::GetGreenColor()
{
	return GetColorID(kDefColor[ kGreenColor ]);
}

JColorID
JColorManager::GetYellowColor()
{
	return GetColorID(kDefColor[ kYellowColor ]);
}

JColorID
JColorManager::GetBlueColor()
{
	return GetColorID(kDefColor[ kBlueColor ]);
}

JColorID
JColorManager::GetMagentaColor()
{
	return GetColorID(kDefColor[ kMagentaColor ]);
}

JColorID
JColorManager::GetCyanColor()
{
	return GetColorID(kDefColor[ kCyanColor ]);
}

JColorID
JColorManager::GetWhiteColor()
{
	return GetColorID(kDefColor[ kWhiteColor ]);
}

JColorID
JColorManager::GetGrayColor
	(
	const JSize percentage
	)
{
	assert( 0 <= percentage && percentage <= 100 );
	const JSize c = JRound(kJMaxRGBValue * (percentage / 100.0));
	return GetColorID(JRGB(c, c, c));
}

JColorID
JColorManager::GetDarkRedColor()
{
	return GetColorID(kDefColor[ kDarkRedColor ]);
}

JColorID
JColorManager::GetOrangeColor()
{
	return GetColorID(kDefColor[ kOrangeColor ]);
}

JColorID
JColorManager::GetDarkGreenColor()
{
	return GetColorID(kDefColor[ kDarkGreenColor ]);
}

JColorID
JColorManager::GetLightBlueColor()
{
	return GetColorID(kDefColor[ kLightBlueColor ]);
}

JColorID
JColorManager::GetBrownColor()
{
	return GetColorID(kDefColor[ kBrownColor ]);
}

JColorID
JColorManager::GetPinkColor()
{
	return GetColorID(kDefColor[ kPinkColor ]);
}

JColorID
JColorManager::GetDefaultSelectionColor()
{
	return GetColorID(kDefColor[ kDefaultSelectionColor ]);
}

JColorID
JColorManager::GetDefaultBackColor()
{
	return GetGrayColor(80);
}

JColorID
JColorManager::GetDefaultFocusColor()
{
	return GetColorID(kDefColor[ kWhiteColor ]);
}

JColorID
JColorManager::GetDefaultSliderBackColor()
{
	return GetGrayColor(70);
}

JColorID
JColorManager::GetInactiveLabelColor()
{
	return GetGrayColor(60);
}

JColorID
JColorManager::GetDefaultSelButtonColor()
{
	return GetColorID(kDefColor[ kWhiteColor ]);
}

JColorID
JColorManager::GetDefaultDNDBorderColor()
{
	return GetColorID(kDefColor[ kBlueColor ]);
}

JColorID
JColorManager::Get3DLightColor()
{
	return GetGrayColor(90);
}

JColorID
JColorManager::Get3DShadeColor()
{
	return GetGrayColor(50);
}
