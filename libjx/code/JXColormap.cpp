/******************************************************************************
 JXColormap.cpp

	Encapsulates the default X Colormap.

	BASE CLASS = JColormap, public JBroadcaster

	Copyright (C) 1996-2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXColormap.h>
#include <JXDisplay.h>
#include <stdlib.h>
#include <jAssert.h>

const JSize kBitsPerColorComp = 16;			// X uses 16 bits for each of r,g,b
	
/******************************************************************************
 Constructor function (static private)

	Called by JXDisplay to create a JXColormap for the default colormap.

 ******************************************************************************/

JXColormap*
JXColormap::Create
	(
	JXDisplay* display
	)
{
	Visual* visual = display->GetDefaultVisual();
	Colormap xColormap =
		DefaultColormap(display->GetXDisplay(), display->GetScreen());

	JXColormap* colormap = jnew JXColormap(display, visual, xColormap);
	assert( colormap != NULL );
	return colormap;
}

/******************************************************************************
 Constructor (private)

 ******************************************************************************/

JXColormap::JXColormap
	(
	JXDisplay*	display,
	Visual*		visual,
	Colormap	xColormap
	)
	:
	JColormap(),
	itsDisplay(display),
	itsXColormap(xColormap),
	itsVisual(visual)
{
	// check if our visual lets us get an empty colormap

	const long vTemplateMask = VisualIDMask | VisualScreenMask;
	XVisualInfo vTemplate;
	vTemplate.visualid = XVisualIDFromVisual(visual);
	vTemplate.screen   = display->GetScreen();

	int count;
	XVisualInfo* vInfo =
		XGetVisualInfo(*display, vTemplateMask, &vTemplate, &count);
	assert( vInfo != NULL );

	itsXVisualInfo = vInfo[0];

	InitMasks(itsXVisualInfo.red_mask,
			  itsXVisualInfo.green_mask,
			  itsXVisualInfo.blue_mask);

	XFree(vInfo);

	assert( itsXVisualInfo.c_class == TrueColor );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXColormap::~JXColormap()
{
}

/******************************************************************************
 InitMasks (private)

 ******************************************************************************/

void
JXColormap::InitMasks
	(
	const unsigned long	redMask,
	const unsigned long	greenMask,
	const unsigned long	blueMask
	)
{
	const short bitCount = 8*sizeof(unsigned long);

	itsMask[0] = redMask;
	itsMask[1] = greenMask;
	itsMask[2] = blueMask;

	for (JIndex i=0; i<3; i++)
		{
		itsStartIndex[i] = 0;
		while (itsStartIndex[i] < bitCount &&
			   (itsMask[i] & (1 << itsStartIndex[i])) == 0)
			{
			itsStartIndex[i]++;
			}
		assert( itsStartIndex[i] < bitCount );

		itsEndIndex[i] = itsStartIndex[i];
		while (itsEndIndex[i] < bitCount &&
			   (itsMask[i] & (1 << itsEndIndex[i])) != 0)
			{
			itsEndIndex[i]++;
			}
		itsEndIndex[i]--;		// point to last 1 bit
		assert( itsEndIndex[i] < bitCount && itsEndIndex[i] >= itsStartIndex[i] );
		}
}

/******************************************************************************
 Useful colors (virtual)

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

JColorIndex
JXColormap::GetBlackColor()
	const
{
	return JColormap::GetColor(kDefColor[ kBlackColor ]);
}

JColorIndex
JXColormap::GetRedColor()
	const
{
	return JColormap::GetColor(kDefColor[ kRedColor ]);
}

JColorIndex
JXColormap::GetGreenColor()
	const
{
	return JColormap::GetColor(kDefColor[ kGreenColor ]);
}

JColorIndex
JXColormap::GetYellowColor()
	const
{
	return JColormap::GetColor(kDefColor[ kYellowColor ]);
}

JColorIndex
JXColormap::GetBlueColor()
	const
{
	return JColormap::GetColor(kDefColor[ kBlueColor ]);
}

JColorIndex
JXColormap::GetMagentaColor()
	const
{
	return JColormap::GetColor(kDefColor[ kMagentaColor ]);
}

JColorIndex
JXColormap::GetCyanColor()
	const
{
	return JColormap::GetColor(kDefColor[ kCyanColor ]);
}

JColorIndex
JXColormap::GetWhiteColor()
	const
{
	return JColormap::GetColor(kDefColor[ kWhiteColor ]);
}

JColorIndex
JXColormap::GetGrayColor
	(
	const JSize percentage
	)
	const
{
	assert( 0 <= percentage && percentage <= 100 );
	const JSize c = JRound(65535 * (percentage / 100.0));
	return GetColor(c, c, c);
}

JColorIndex
JXColormap::GetDarkRedColor()
	const
{
	return JColormap::GetColor(kDefColor[ kDarkRedColor ]);
}

JColorIndex
JXColormap::GetOrangeColor()
	const
{
	return JColormap::GetColor(kDefColor[ kOrangeColor ]);
}

JColorIndex
JXColormap::GetDarkGreenColor()
	const
{
	return JColormap::GetColor(kDefColor[ kDarkGreenColor ]);
}

JColorIndex
JXColormap::GetLightBlueColor()
	const
{
	return JColormap::GetColor(kDefColor[ kLightBlueColor ]);
}

JColorIndex
JXColormap::GetBrownColor()
	const
{
	return JColormap::GetColor(kDefColor[ kBrownColor ]);
}

JColorIndex
JXColormap::GetPinkColor()
	const
{
	return JColormap::GetColor(kDefColor[ kPinkColor ]);
}

JColorIndex
JXColormap::GetDefaultSelectionColor()
	const
{
	return JColormap::GetColor(kDefColor[ kDefaultSelectionColor ]);
}

JColorIndex
JXColormap::GetDefaultBackColor()
	const
{
	return GetGrayColor(80);
}

JColorIndex
JXColormap::GetDefaultFocusColor()
	const
{
	return JColormap::GetColor(kDefColor[ kWhiteColor ]);
}

JColorIndex
JXColormap::GetDefaultSliderBackColor()
	const
{
	return GetGrayColor(70);
}

JColorIndex
JXColormap::GetInactiveLabelColor()
	const
{
	return GetGrayColor(60);
}

JColorIndex
JXColormap::GetDefaultSelButtonColor()
	const
{
	return JColormap::GetColor(kDefColor[ kWhiteColor ]);
}

JColorIndex
JXColormap::GetDefaultDNDBorderColor()
	const
{
	return JColormap::GetColor(kDefColor[ kBlueColor ]);
}

JColorIndex
JXColormap::Get3DLightColor()
	const
{
	return GetGrayColor(90);
}

JColorIndex
JXColormap::Get3DShadeColor()
	const
{
	return GetGrayColor(50);
}

/******************************************************************************
 GetColor (virtual)

	Returns kJTrue if it was able to allocate the requested read-only color.
	Accepts both names from the color database and hex specifications.
	(i.e. "red" or "#FFFF00000000")

 ******************************************************************************/

JBoolean
JXColormap::GetColor
	(
	const JString&	name,
	JColorIndex*	colorIndex
	)
	const
{
	XColor xColor;
	if (XParseColor(*itsDisplay, itsXColormap, name.GetBytes(), &xColor))
		{
		*colorIndex = GetColor(xColor.red, xColor.green, xColor.blue);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetColor (virtual)

	We scale each component to fit inside the number of bits in its mask.
	This means multiplying by (2^(mask length)-1)/(2^16-1).  By ignoring
	the -1's, it becomes a bit shift.  The bit shift produces cleaner
	levels of gray with 16-bit color and negligible difference on 24-bit
	color.  Both produce inaccurate results when writing GIFs on 16-bit
	color.

 ******************************************************************************/

JColorIndex
JXColormap::GetColor
	(
	const JSize	red,
	const JSize	green,
	const JSize	blue
	)
	const
{
	const unsigned long rgb[3] = { red, green, blue };

	unsigned long xPixel = 0;
	for (JIndex i=0; i<3; i++)
		{
		const short shift = itsEndIndex[i] + 1 - kBitsPerColorComp;
		if (shift >= 0)
			{
			xPixel |= (rgb[i] << shift) & itsMask[i];
			}
		else
			{
			xPixel |= (rgb[i] >> -shift) & itsMask[i];
			}
		}

	return xPixel;
}

/******************************************************************************
 GetRGB (virtual)

	When all colors are preallocated, we scale each component up so that a
	full mask becomes 2^16-1.

 ******************************************************************************/

void
JXColormap::GetRGB
	(
	const JColorIndex	colorIndex,
	JSize*				red,
	JSize*				green,
	JSize*				blue
	)
	const
{
	unsigned long rgb[3];
	for (JIndex i=0; i<3; i++)
		{
		rgb[i] = (colorIndex & itsMask[i]) >> itsStartIndex[i];
		if (itsStartIndex[i] > 0)
			{
			rgb[i] &= ~(~(0L) << (8*sizeof(unsigned long) - itsStartIndex[i]));
			}
		const short maxValue = (1 << (itsEndIndex[i]-itsStartIndex[i]+1)) - 1;
		rgb[i] = JRound(rgb[i] * kJMaxRGBValueF / maxValue);
		}

	*red   = rgb[0];
	*green = rgb[1];
	*blue  = rgb[2];
}
