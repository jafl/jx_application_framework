/******************************************************************************
 JXColorManager.cpp

	Encapsulates the default X Colormap.

	BASE CLASS = JColorManager, public JBroadcaster

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/JXDisplay.h"
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

const JSize kBitsPerColorComp = 16;			// X uses 16 bits for each of r,g,b
	
/******************************************************************************
 Constructor function (static private)

	Called by JXDisplay to create a JXColorManager for the default colormap.

 ******************************************************************************/

JXColorManager*
JXColorManager::Create
	(
	JXDisplay* display
	)
{
	Visual* visual = display->GetDefaultVisual();
	Colormap xColormap =
		DefaultColormap(display->GetXDisplay(), display->GetScreen());

	auto* colormap = jnew JXColorManager(display, visual, xColormap);
	assert( colormap != nullptr );
	return colormap;
}

/******************************************************************************
 Constructor (private)

 ******************************************************************************/

JXColorManager::JXColorManager
	(
	JXDisplay*	display,
	Visual*		visual,
	Colormap	xColormap
	)
	:
	JColorManager(),
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
	assert( vInfo != nullptr );

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

JXColorManager::~JXColorManager()
{
}

/******************************************************************************
 InitMasks (private)

 ******************************************************************************/

void
JXColorManager::InitMasks
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

	for (const JIndex i : { 0,1,2 })
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
 GetColorID (virtual)

	Returns true if it was able to allocate the requested read-only color.
	Accepts both names from the color database and hex specifications.
	(i.e. "red" or "#FFFF00000000")

 ******************************************************************************/

bool
JXColorManager::GetColorID
	(
	const JString&	name,
	JColorID*		id
	)
	const
{
	XColor xColor;
	if (XParseColor(*itsDisplay, itsXColormap, name.GetBytes(), &xColor))
	{
		*id = JColorManager::GetColorID(JRGB(xColor.red, xColor.green, xColor.blue));
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetColorID

	Converts a system color value into a ColorID

 ******************************************************************************/

JColorID
JXColorManager::GetColorID
	(
	const unsigned long systemColor
	)
	const
{
	unsigned long rgb[3];
	for (const JIndex i : { 0,1,2 })
	{
		rgb[i] = (systemColor & itsMask[i]) >> itsStartIndex[i];
		if (itsStartIndex[i] > 0)
		{
			rgb[i] &= ~(~(0L) << (8*sizeof(unsigned long) - itsStartIndex[i]));
		}
		const short maxValue = (1 << (itsEndIndex[i]-itsStartIndex[i]+1)) - 1;
		rgb[i] = JRound(rgb[i] * kJMaxRGBValueF / maxValue);
	}

	return JColorManager::GetColorID(JRGB(rgb[0], rgb[1], rgb[2]));
}

/******************************************************************************
 GetXColor

	We scale each component to fit inside the number of bits in its mask.
	This means multiplying by (2^(mask length)-1)/(2^16-1).  By ignoring
	the -1's, it becomes a bit shift.  The bit shift produces cleaner
	levels of gray with 16-bit color and negligible difference on 24-bit
	color.  Both produce inaccurate results when writing GIFs on 16-bit
	color.

 ******************************************************************************/

unsigned long
JXColorManager::GetXColor
	(
	const JColorID id
	)
	const
{
	const JRGB c               = GetRGB(id);
	const unsigned long rgb[3] = { c.red, c.green, c.blue };

	unsigned long xPixel = 0;
	for (const JIndex i : { 0,1,2 })
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
