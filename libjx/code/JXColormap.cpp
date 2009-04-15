/******************************************************************************
 JXColormap.cpp

	Represents an X Colormap.  Color approximation can be done either
	preemptively (look for a close color before trying to allocate a new one)
	or only after the colormap is full (the default).

	Since JXDisplay can switch to a private colormap without notice,
	we hide colors behind JColorIndex.

	When the JXDisplay's colormap changes to a private colormap, the new
	colormap uses the same Visual because an existing X window cannot switch
	Visuals.

	We cannot sort the color array because the indices that we give out must
	remain valid.  Appending new items is the only option.  This is not much of
	a problem, however, both because the most common colors have predefined
	indices, and because looking up colors only has to be done once in each
	constructor.

	We do not have to keep a separate usage count for each X pixel value
	because XFreeColors() only works on read/write colormaps, and these
	colormaps never perform color approximation.

	With PseudoColor, all X pixel values are between 0 and 255.
	For TrueColor, the X pixel values can be calculated from the masks.
	For DirectColor, the X pixel values seem to be arbitrary unsigned longs.

	To do:

		Does not use preemptive allocation while switching to a private
		colormap.

	BASE CLASS = JColormap, virtual JBroadcaster

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXColormap.h>
#include <JXDisplay.h>
#include <stdlib.h>
#include <jAssert.h>

const JSize kBitsPerColorComp   = 16;			// X uses 16 bits for each of r,g,b
const JSize kColorListBlockSize = 30;

const JCoordinate kDefColorBoxHW    = 40000;	// recommended by XPM docs
const JCoordinate kDefPreColorBoxHW = 4000;

// JBroadcaster message types

const JCharacter* JXColormap::kNewColormap = "NewColormap::JXColormap";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about failures within the class itself.

 ******************************************************************************/

JBoolean
JXColormap::Create
	(
	JXDisplay*		display,
	const int		visualClass,
	JXColormap**	colormap
	)
{
	*colormap = NULL;

	// find the visual to use

	const long vTemplateMask = VisualScreenMask | VisualClassMask;
	XVisualInfo vTemplate;
	vTemplate.screen  = display->GetScreen();
	vTemplate.c_class = visualClass;

	int count;
	XVisualInfo* vInfo =
		XGetVisualInfo(*display, vTemplateMask, &vTemplate, &count);
	if (vInfo == NULL)
		{
		return kJFalse;
		}

	Visual* visual = vInfo[0].visual;
	XFree(vInfo);

	// create the X colormap

	Colormap xColormap =
		XCreateColormap(*display, display->GetRootWindow(), visual, AllocNone);
	if (xColormap == DefaultColormap(display->GetXDisplay(), display->GetScreen()))
		{
		// custom colormap not possible
		return kJFalse;
		}

	// create the object

	*colormap = new JXColormap(display, visual, xColormap, kJTrue);
	return JConvertToBoolean( *colormap != NULL );
}

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

	JXColormap* colormap = new JXColormap(display, visual, xColormap, kJFalse);
	assert( colormap != NULL );
	return colormap;
}

/******************************************************************************
 Constructor (private)

 ******************************************************************************/

JXColormap::JXColormap
	(
	JXDisplay*		display,
	Visual*			visual,
	Colormap		xColormap,
	const JBoolean	ownsColormap
	)
	:
	JColormap()
{
	itsDisplay               = display;
	itsXColormap             = xColormap;
	itsVisual                = visual;
	itsAllowApproxColorsFlag = kJTrue;
	itsPreApproxColorsFlag   = kJFalse;
	itsOwnsColormapFlag      = ownsColormap;
	itsCanSwitchCmapFlag     = !ownsColormap;
	itsNotifyNewCmapFlag     = kJFalse;
	itsSwitchingCmapFlag     = kJFalse;
	itsColorList             = NULL;
	itsColorDistanceFn       = XPMColorDistance;
	itsColorBoxHW            = kDefColorBoxHW;
	itsPreColorBoxHW         = kDefPreColorBoxHW;
	itsXColorListUseCount    = 0;
	itsXColorListInitFlag    = kJFalse;
	itsXColorList            = NULL;

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

	InitPreallocInfo(itsXVisualInfo.red_mask,
					 itsXVisualInfo.green_mask,
					 itsXVisualInfo.blue_mask);

	XFree(vInfo);

	const int vc = itsXVisualInfo.c_class;
	if (vc == StaticGray || vc == StaticColor ||
		vc == TrueColor  || vc == DirectColor)
		{
		// With DirectColor, we will never need to switch to a new colormap.

		itsCanSwitchCmapFlag = kJFalse;
		}
	itsOrigCanSwitchCmapFlag = itsCanSwitchCmapFlag;

	itsAllColorsPreallocatedFlag = JConvertToBoolean(
		vc == TrueColor || vc == StaticColor || vc == StaticGray );

	// allocate useful colors

	AllocateDefaultColors();

	// now that we have been created, changes have to be broadcast

	itsNotifyNewCmapFlag = kJTrue;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXColormap::~JXColormap()
{
	if (itsOwnsColormapFlag)
		{
		XFreeColormap(*itsDisplay, itsXColormap);
		}
	else if (itsColorList != NULL)
		{
		const JSize colorCount = itsColorList->GetElementCount();
		for (JIndex i=1; i<=colorCount; i++)
			{
			ColorInfo info = itsColorList->GetElement(i);
			if (info.useCount > 0)
				{
				XFreeColors(*itsDisplay, itsXColormap, &(info.xPixel), 1, 0);
				}
			}
		}

	delete itsColorList;
	delete [] itsXColorList;
}

/******************************************************************************
 AllocateDefaultColors

	This function will never fail because:
		1)	If the visual allows allocating colors, and we run out of
			space, we will create a new, empty colormap, and then we won't
			run out of space.
		2)  If the visual doesn't allow allocating colors, all the colors
			already exist, so we will never run out of space.

	We try our best to allocate only Netscape-compatible colors:
		0, 13107, 26214, 39321, 52428, 65535

 ******************************************************************************/

enum
{
	kBlackColor = 1,
	kRedColor,
	kGreenColor,
	kYellowColor,
	kBlueColor,
	kMagentaColor,
	kCyanColor,
	kWhiteColor,

	kGray10Color,
	kGray20Color,
	kGray30Color,
	kGray40Color,
	kGray50Color,
	kGray60Color,
	kGray70Color,
	kGray80Color,
	kGray90Color,

	kGray25Color,
	kGray75Color,

	kDarkRedColor,
	kOrangeColor,
	kDarkGreenColor,
	kLightBlueColor,
	kBrownColor,
	kPinkColor,

	kDefaultSelectionColor,

	kLastPredefColor = kDefaultSelectionColor,

	// useful aliases

	kDefaultBackColor       = kGray80Color,
	kDefaultFocusColor      = kWhiteColor,
	kDefaultSliderBackColor = kGray70Color,
	kInactiveLabelColor     = kGray60Color,
	kDefaultSelButtonColor  = kWhiteColor,
	kDefaultDNDBorderColor  = kBlueColor,

	k3DLightColor = kGray90Color,
	k3DShadeColor = kGray50Color
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

	JRGB(0x1999, 0x1999, 0x1999),		// gray 10
	JRGB(0x3333, 0x3333, 0x3333),		// gray 20
	JRGB(0x4CCC, 0x4CCC, 0x4CCC),		// gray 30
	JRGB(0x6666, 0x6666, 0x6666),		// gray 40
	JRGB(0x8000, 0x8000, 0x8000),		// gray 50
	JRGB(0x9999, 0x9999, 0x9999),		// gray 60
	JRGB(0xB333, 0xB333, 0xB333),		// gray 70
	JRGB(0xCCCC, 0xCCCC, 0xCCCC),		// gray 80
	JRGB(0xE666, 0xE666, 0xE666),		// gray 90

	JRGB(0x4000, 0x4000, 0x4000),		// gray 25
	JRGB(0xC000, 0xC000, 0xC000),		// gray 75

	JRGB(0x9999,      0,      0),		// dark red
	JRGB(0xFFFF, 0x9999,      0),		// orange
	JRGB(     0, 0x6666,      0),		// dark green
	JRGB(     0, 0x9999, 0xFFFF),		// light blue
	JRGB(0x9999, 0x6666,      0),		// brown
	JRGB(0xFFFF,      0, 0x9999),		// pink

	JRGB(0x9999, 0xFFFF, 0xFFFF)		// selection hilighting
};

const JSize kDefColorCount = sizeof(kDefColor)/sizeof(JRGB);

void
JXColormap::AllocateDefaultColors()
{
	assert( kDefColorCount == kLastPredefColor );

	if (!itsAllColorsPreallocatedFlag)
		{
		itsColorList = new JArray<ColorInfo>(kColorListBlockSize);
		assert( itsColorList != NULL );

		AllocateXColorList();

		for (JIndex i=1; i<=kDefColorCount; i++)
			{
			JColorIndex colorIndex;
			const JBoolean ok =
				JColormap::AllocateStaticColor(kDefColor[i-1], &colorIndex);
			assert( ok && colorIndex == i );
			}

		FreeXColorList();
		}
}

/******************************************************************************
 Pre-allocated colors (virtual)

 ******************************************************************************/

JColorIndex
JXColormap::GetBlackColor()
	const
{
	#define ColorIndex	kBlackColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetRedColor()
	const
{
	#define ColorIndex	kRedColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGreenColor()
	const
{
	#define ColorIndex	kGreenColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetYellowColor()
	const
{
	#define ColorIndex	kYellowColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetBlueColor()
	const
{
	#define ColorIndex	kBlueColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetMagentaColor()
	const
{
	#define ColorIndex	kMagentaColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetCyanColor()
	const
{
	#define ColorIndex	kCyanColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetWhiteColor()
	const
{
	#define ColorIndex	kWhiteColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray10Color()
	const
{
	#define ColorIndex	kGray10Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray20Color()
	const
{
	#define ColorIndex	kGray20Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray30Color()
	const
{
	#define ColorIndex	kGray30Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray40Color()
	const
{
	#define ColorIndex	kGray40Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray50Color()
	const
{
	#define ColorIndex	kGray50Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray60Color()
	const
{
	#define ColorIndex	kGray60Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray70Color()
	const
{
	#define ColorIndex	kGray70Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray80Color()
	const
{
	#define ColorIndex	kGray80Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray90Color()
	const
{
	#define ColorIndex	kGray90Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray25Color()
	const
{
	#define ColorIndex	kGray25Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetGray75Color()
	const
{
	#define ColorIndex	kGray75Color
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDarkRedColor()
	const
{
	#define ColorIndex	kDarkRedColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetOrangeColor()
	const
{
	#define ColorIndex	kOrangeColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDarkGreenColor()
	const
{
	#define ColorIndex	kDarkGreenColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetLightBlueColor()
	const
{
	#define ColorIndex	kLightBlueColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetBrownColor()
	const
{
	#define ColorIndex	kBrownColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetPinkColor()
	const
{
	#define ColorIndex	kPinkColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultSelectionColor()
	const
{
	#define ColorIndex	kDefaultSelectionColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultBackColor()
	const
{
	#define ColorIndex	kDefaultBackColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultFocusColor()
	const
{
	#define ColorIndex	kDefaultFocusColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultSliderBackColor()
	const
{
	#define ColorIndex	kDefaultSliderBackColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetInactiveLabelColor()
	const
{
	#define ColorIndex	kInactiveLabelColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultSelButtonColor()
	const
{
	#define ColorIndex	kDefaultSelButtonColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::GetDefaultDNDBorderColor()
	const
{
	#define ColorIndex	kDefaultDNDBorderColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::Get3DLightColor()
	const
{
	#define ColorIndex	k3DLightColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

JColorIndex
JXColormap::Get3DShadeColor()
	const
{
	#define ColorIndex	k3DShadeColor
	#include <JXGetPreallocColor.th>
	#undef ColorIndex
}

/******************************************************************************
 AllColorsPreallocated (virtual)

 ******************************************************************************/

JBoolean
JXColormap::AllColorsPreallocated()
	const
{
	return itsAllColorsPreallocatedFlag;
}

/******************************************************************************
 Mass color allocation (virtual)

 ******************************************************************************/

void
JXColormap::PrepareForMassColorAllocation()
{
	AllocateXColorList();
}

void
JXColormap::MassColorAllocationFinished()
{
	FreeXColorList();
}

/******************************************************************************
 AllocateStaticNamedColor (virtual)

	Returns kJTrue if it was able to allocate the requested read-only color.
	Accepts both names from the color database and hex specifications.
	(i.e. "red" or "#FFFF00000000")

 ******************************************************************************/

JBoolean
JXColormap::AllocateStaticNamedColor
	(
	const JCharacter*	name,
	JColorIndex*		colorIndex
	)
{
	XColor xColor;
	if (XParseColor(*itsDisplay, itsXColormap, name, &xColor))
		{
		return AllocateStaticColor(xColor.red, xColor.green, xColor.blue, colorIndex);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 AllocateStaticColor (virtual)

	Returns kJTrue if it was able to allocate the requested read-only color.

	We only increase the usage count if we find an exact match because
	XFreeColors() is only functional for read/write colormaps, and these
	colormaps only return the same xPixel when the colors match exactly.
	(i.e. The only time we will get the same xPixel for different rgb values
	      is when we have a read-only colormap, and then XFreeColors()
	      (and hence usage counts for xPixels) is irrelevant.)

	exactMatch can be NULL.

 ******************************************************************************/

JBoolean
JXColormap::AllocateStaticColor
	(
	const JSize		red,
	const JSize		green,
	const JSize		blue,
	JColorIndex*	colorIndex,
	JBoolean*		exactMatch
	)
{
	if (itsAllColorsPreallocatedFlag)
		{
		const JBoolean ok = CalcPreallocatedXPixel(red, green, blue, colorIndex);
		if (ok && exactMatch != NULL)
			{
			const JRGB color = JColormap::GetRGB(*colorIndex);
			*exactMatch = JConvertToBoolean(
				red == color.red && green == color.green && blue == color.blue );
			}
		return ok;
		}

	assert( itsColorList != NULL );

	JColorIndex closestColorIndex = 0;

	if (!itsSwitchingCmapFlag)
		{
		const JSize colorCount = itsColorList->GetElementCount();
		JFloat minDistance     = 0.0;
		for (JIndex i=1; i<=colorCount; i++)
			{
			ColorInfo info = itsColorList->GetElement(i);
			if (info.useCount > 0 && !info.dynamic && !info.preemptive)
				{
				if (info.color.red   == red &&
					info.color.green == green &&
					info.color.blue  == blue)
					{
					(info.useCount)++;
					itsColorList->SetElement(i, info);

					*colorIndex = i;
					if (exactMatch != NULL)
						{
						*exactMatch = info.exactMatch;
						}
					return kJTrue;
					}
				else if (itsAllowApproxColorsFlag && itsPreApproxColorsFlag &&
						 IsInsideColorBox(red, green, blue,
						 				  info.color.red, info.color.green, info.color.blue,
						 				  itsPreColorBoxHW))
					{
					const JFloat d =
						itsColorDistanceFn(red, green, blue,
										   info.color.red, info.color.green, info.color.blue);
					if (d < minDistance || closestColorIndex == 0)
						{
						minDistance       = d;
						closestColorIndex = i;
						}
					}
				}
			}
		}

	if (closestColorIndex > 0)
		{
		const ColorInfo info = itsColorList->GetElement(closestColorIndex);

		// increment X server's usage count

		XColor xColor;
		xColor.pixel = info.xPixel;
		XQueryColor(*itsDisplay, itsXColormap, &xColor);
		const Status ok = XAllocColor(*itsDisplay, itsXColormap, &xColor);
		assert( ok && xColor.pixel == info.xPixel );

		*colorIndex = StoreNewColor(
			ColorInfo(red, green, blue, info.xPixel, kJFalse, kJFalse, kJTrue));
		if (exactMatch != NULL)
			{
			*exactMatch = kJFalse;
			}
		return kJTrue;
		}
	else if (PrivateAllocateStaticColor(red, green, blue, colorIndex, exactMatch) ||
			 AllocateApproxStaticColor(red, green, blue, colorIndex, exactMatch))
		{
		return kJTrue;
		}
	else if (CreateEmptyColormap())
		{
		assert( itsOwnsColormapFlag );		// recurse only once
		return AllocateStaticColor(red, green, blue, colorIndex, exactMatch);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 AllocateXColorList (private)

	Allocate the color list so it can be used by AllocateApproxStaticColor().
	When finished, call FreeXColorList().

 ******************************************************************************/

void
JXColormap::AllocateXColorList()
{
	if (itsXColorListUseCount == 0)
		{
		itsXColorListInitFlag = kJFalse;

		itsXColorList = new XColor [ GetMaxColorCount() ];
		assert( itsXColorList != NULL );
		}

	itsXColorListUseCount++;
}

/******************************************************************************
 FreeXColorList (private)

 ******************************************************************************/

void
JXColormap::FreeXColorList()
{
	if (itsXColorListUseCount > 0)
		{
		itsXColorListUseCount--;

		if (itsXColorListUseCount == 0)
			{
			delete [] itsXColorList;
			itsXColorList = NULL;
			}
		}
}

/******************************************************************************
 AllocateApproxStaticColor (private)

	Given that the colormap is full, searches for the closest existing color.

	The ideas are straight from libXpm version 4.11, create.c, SetCloseColor().

	If the color is a shade of gray, only other grays are considered when
	looking for the closest color.

 ******************************************************************************/

const JSize kIterCount = 3;		// last iteration is done while server is grabbed

struct CloseColorInfo
{
	unsigned long	xPixel;
	long			d;
};

static int CompareCloseColorInfo(const void* i1, const void* i2);

int
CompareCloseColorInfo
	(
	const void* i1,
	const void* i2
	)
{
	return ((CloseColorInfo*) i1)->d - ((CloseColorInfo*) i2)->d;
}

inline int
IsGray
	(
	const JSize red,
	const JSize green,
	const JSize blue
	)
{
	return (red == green && green == blue);
}

JBoolean
JXColormap::AllocateApproxStaticColor
	(
	const JSize		red,
	const JSize		green,
	const JSize		blue,
	JColorIndex*	colorIndex,
	JBoolean*		exactMatch
	)
{
JIndex i;

	if (itsAllColorsPreallocatedFlag || !itsAllowApproxColorsFlag)
		{
		return kJFalse;
		}

	const JSize colorCount = GetMaxColorCount();

	// get the set of allocated colors

	XColor* xColor        = itsXColorList;
	JBoolean deleteXColor = kJFalse;
	if (itsXColorList == NULL)
		{
		xColor = new XColor [ colorCount ];
		assert( xColor != NULL );
		deleteXColor = kJTrue;
		}

	if (itsXColorList == NULL || !itsXColorListInitFlag)
		{
		for (i=0; i<colorCount; i++)
			{
			xColor[i].pixel = i;
			}
		XQueryColors(*itsDisplay, itsXColormap, xColor, colorCount);
		itsXColorListInitFlag = kJTrue;
		}

	// search for the closest usable color

	CloseColorInfo* info = new CloseColorInfo [ colorCount ];
	assert( info != NULL );

	for (JIndex iterIndex=1; iterIndex<=kIterCount; iterIndex++)
		{
		JBoolean serverGrabbed = kJFalse;
		if (iterIndex == kIterCount)
			{
			serverGrabbed = kJTrue;
			XGrabServer(*itsDisplay);
			}
		if (iterIndex > 1)		// pre-loop logic checks if necessary the first time
			{
			XQueryColors(*itsDisplay, itsXColormap, xColor, colorCount);
			}

		// sort the list of colors by closeness

		for (i=0; i<colorCount; i++)
			{
			info[i].xPixel = i;
			info[i].d =
				itsColorDistanceFn(red, green, blue,
								   xColor[i].red, xColor[i].green, xColor[i].blue);
			}
		qsort(info, colorCount, sizeof(CloseColorInfo), CompareCloseColorInfo);

		// find the closest usable color (colormap may change or pixel may be read/write)

		const JBoolean forceMatch = kJFalse;
		for (i=0; i<colorCount; i++)
			{
			XColor* c = xColor + info[i].xPixel;
			if (IsInsideColorBox(red, green, blue,
								 c->red, c->green, c->blue, itsColorBoxHW) &&
				(!IsGray(red, green, blue) || IsGray(c->red, c->green, c->blue)) &&
				PrivateAllocateStaticColor(c->red, c->green, c->blue,
										   colorIndex, exactMatch, &forceMatch))
				{
				if (serverGrabbed)
					{
					XUngrabServer(*itsDisplay);
					}
				delete [] info;
				if (deleteXColor)
					{
					delete [] xColor;
					}
				return kJTrue;
				}
			}

		if (serverGrabbed)
			{
			XUngrabServer(*itsDisplay);
			}

		// The colormap probably changed while we were busy, so we try
		// the original color again.

		if (PrivateAllocateStaticColor(red, green, blue, colorIndex, exactMatch))
			{
			delete [] info;
			if (deleteXColor)
				{
				delete [] xColor;
				}
			return kJTrue;
			}
		}

	delete [] info;
	if (deleteXColor)
		{
		delete [] xColor;
		}
	return kJFalse;
}

/******************************************************************************
 XPMColorDistance (static)

	The distance function used by the XPM library.

 ******************************************************************************/

long
JXColormap::XPMColorDistance
	(
	const long r1,
	const long g1,
	const long b1,
	const long r2,
	const long g2,
	const long b2
	)
{
	const long kColorFactor      = 3;		// weighting factor for color
	const long kBrightnessFactor = 1;		// weighting factor for brightness

	return kColorFactor * (abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2)) +
			kBrightnessFactor * abs((r1 + g1 + b1) - (r2 + g2 + b2));
}

/******************************************************************************
 PrivateAllocateStaticColor (private)

	Returns kJTrue if it was able to allocate the requested read-only color.

	exactMatch can be NULL.

	If forceMatch != NULL, *exactMatch = *forceMatch.

 ******************************************************************************/

JBoolean
JXColormap::PrivateAllocateStaticColor
	(
	const JSize		red,
	const JSize		green,
	const JSize		blue,
	JColorIndex*	colorIndex,
	JBoolean*		exactMatch,
	const JBoolean*	forceMatch
	)
{
	XColor xColor;
	xColor.red   = red;
	xColor.green = green;
	xColor.blue  = blue;
	xColor.flags = DoRed | DoGreen | DoBlue;

	if (XAllocColor(*itsDisplay, itsXColormap, &xColor))
		{
		const JBoolean match =
			forceMatch != NULL ?
				*forceMatch :
				JI2B(red == xColor.red && green == xColor.green && blue == xColor.blue );

		const ColorInfo info(red, green, blue, xColor.pixel, match, kJFalse, kJFalse);
		*colorIndex = StoreNewColor(info);

		if (exactMatch != NULL)
			{
			*exactMatch = match;
			}
		return kJTrue;
		}
	else
		{
		*colorIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 AllocateStaticColor

	Allocates a JColorIndex for the given read-only X pixel value.
	The X pixel value must already have been allocated!

 ******************************************************************************/

void
JXColormap::AllocateStaticColor
	(
	const unsigned long	xPixel,
	JColorIndex*		colorIndex
	)
{
	if (!GetColorIndex(xPixel, colorIndex))
		{
		XColor xColor;
		xColor.pixel = xPixel;
		XQueryColor(*itsDisplay, itsXColormap, &xColor);

		const JBoolean saveAllow = itsAllowApproxColorsFlag;
		itsAllowApproxColorsFlag = kJFalse;

		JBoolean exactMatch;
		const JBoolean ok =
			AllocateStaticColor(xColor.red, xColor.green, xColor.blue,
								colorIndex, &exactMatch);
		assert( ok && exactMatch );

		itsAllowApproxColorsFlag = saveAllow;
		}
}

/******************************************************************************
 InitPreallocInfo (private)

 ******************************************************************************/

void
JXColormap::InitPreallocInfo
	(
	const unsigned long	redMask,
	const unsigned long	greenMask,
	const unsigned long	blueMask
	)
{
	if (redMask != 0 && greenMask != 0 && blueMask != 0)
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
}

/******************************************************************************
 CalcPreallocatedXPixel

	Returns kJTrue if it was able to calculate the xPixel value of the given
	color without contacting the server.

	We ought to scale each component to fit inside the number of bits in
	its mask.  This means multiplying by (2^(mask length)-1)/(2^16-1).  By
	ignoring the -1's, it becomes a bit shift.  The bit shift produces
	cleaner levels of gray with 16-bit color and negligible difference on
	24-bit color.  Both produce inaccurate results when writing GIFs on
	16-bit color.

 ******************************************************************************/

JBoolean
JXColormap::CalcPreallocatedXPixel
	(
	const JSize		red,
	const JSize		green,
	const JSize		blue,
	unsigned long*	xPixel
	)
	const
{
	if (itsAllColorsPreallocatedFlag)
		{
		const unsigned long rgb[3] = { red, green, blue };

		*xPixel = 0;
		for (JIndex i=0; i<3; i++)
			{
			const short shift = itsEndIndex[i] + 1 - kBitsPerColorComp;
			if (shift >= 0)
				{
				*xPixel |= (rgb[i] << shift) & itsMask[i];
				}
			else
				{
				*xPixel |= (rgb[i] >> -shift) & itsMask[i];
				}

//			const short maxValue    = (1 << (itsEndIndex[i]-itsStartIndex[i]+1)) - 1;
//			const JSize scaledValue = JRound((rgb[i] * maxValue) / kJMaxRGBValueF);
//			*xPixel |= (scaledValue << itsStartIndex[i]) & itsMask[i];
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CanAllocateDynamicColors (virtual)

 ******************************************************************************/

JBoolean
JXColormap::CanAllocateDynamicColors()
	const
{
	const int vc = GetVisualClass();
	return JConvertToBoolean(
		vc == DirectColor || vc == PseudoColor || vc == GrayScale );
}

/******************************************************************************
 AllocateDynamicColor (virtual)

	Returns kJTrue if it was able to allocate the requested read/write color.
	We do not provide a way to allocate more than one at a time because
	writing the code to allocate one at a time makes it easy to break out
	if one runs out of space in the colormap.

 ******************************************************************************/

JBoolean
JXColormap::AllocateDynamicColor
	(
	const JSize		red,
	const JSize		green,
	const JSize		blue,
	JColorIndex*	colorIndex
	)
{
	if (!CanAllocateDynamicColors())
		{
		return kJFalse;
		}

	unsigned long xPixel;
	if (XAllocColorCells(*itsDisplay, itsXColormap, False, NULL, 0, &xPixel, 1))
		{
		const ColorInfo info(red, green, blue, xPixel, kJTrue, kJTrue, kJFalse);
		*colorIndex = StoreNewColor(info);

		XColor xColor;
		xColor.pixel = xPixel;
		xColor.red   = red;
		xColor.green = green;
		xColor.blue  = blue;
		xColor.flags = DoRed | DoGreen | DoBlue;
		XStoreColor(*itsDisplay, itsXColormap, &xColor);

		return kJTrue;
		}
	else if (CreateEmptyColormap())
		{
		assert( itsOwnsColormapFlag );		// recurse only once
		return AllocateDynamicColor(red, green, blue, colorIndex);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetDynamicColor (virtual)

	Sets the rgb components for the specified color.

 ******************************************************************************/

void
JXColormap::SetDynamicColor
	(
	const JColorIndex	colorIndex,
	const JSize			red,
	const JSize			green,
	const JSize			blue
	)
{
	assert( itsColorList != NULL );

	ColorInfo info = itsColorList->GetElement(colorIndex);
	assert( info.dynamic );
	info.color.red   = red;
	info.color.green = green;
	info.color.blue  = blue;
	itsColorList->SetElement(colorIndex, info);

	XColor xColor;
	xColor.pixel = info.xPixel;
	xColor.red   = red;
	xColor.green = green;
	xColor.blue  = blue;
	xColor.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(*itsDisplay, itsXColormap, &xColor);
}

/******************************************************************************
 SetDynamicColors (virtual)

	Sets the rgb components for the specified colors.  This is provided because
	extra work on the client side is faster than multiple calls to the
	server.  This also changes all the colors simultaneously, instead of one
	at a time, which may be required for some animation effects.

 ******************************************************************************/

void
JXColormap::SetDynamicColors
	(
	const JArray<JDynamicColorInfo>& colorList
	)
{
	assert( itsColorList != NULL );

	const JSize count = colorList.GetElementCount();

	XColor* xColor = new XColor[count];
	assert( xColor != NULL );

	for (JIndex i=0; i<count; i++)
		{
		const JDynamicColorInfo dcinfo = colorList.GetElement(i+1);

		ColorInfo info = itsColorList->GetElement(dcinfo.index);
		assert( info.dynamic );
		info.color = dcinfo.color;
		itsColorList->SetElement(dcinfo.index, info);

		xColor[i].pixel = info.xPixel;
		xColor[i].red   = info.color.red;
		xColor[i].green = info.color.green;
		xColor[i].blue  = info.color.blue;
		xColor[i].flags = DoRed | DoGreen | DoBlue;
		}

	XStoreColors(*itsDisplay, itsXColormap, xColor, count);

	delete [] xColor;
}

/******************************************************************************
 UsingColor (virtual)

	Call this if you didn't allocate a JColorIndex but you need it anyway.
	This insures that the color will exist until you call DeallocateColor().

 ******************************************************************************/

void
JXColormap::UsingColor
	(
	const JColorIndex colorIndex
	)
{
	if (!itsAllColorsPreallocatedFlag && colorIndex > kLastPredefColor)
		{
		assert( itsColorList != NULL );
		ColorInfo info = itsColorList->GetElement(colorIndex);
		assert( info.useCount > 0 );
		(info.useCount)++;
		itsColorList->SetElement(colorIndex, info);
		}
}

/******************************************************************************
 DeallocateColor (virtual)

 ******************************************************************************/

void
JXColormap::DeallocateColor
	(
	const JColorIndex colorIndex
	)
{
	if (!itsAllColorsPreallocatedFlag && colorIndex > kLastPredefColor)
		{
		assert( itsColorList != NULL );

		ColorInfo info = itsColorList->GetElement(colorIndex);
		if (info.useCount > 0)
			{
			(info.useCount)--;
			itsColorList->SetElement(colorIndex, info);
			if (info.useCount == 0)
				{
				XFreeColors(*itsDisplay, itsXColormap, &(info.xPixel), 1, 0);

				// strip empty entries off the end of the list

				info = itsColorList->GetLastElement();
				while (info.useCount == 0)
					{
					itsColorList->RemoveElement(itsColorList->GetElementCount());
					info = itsColorList->GetLastElement();
					}
				}
			}
		}
}

/******************************************************************************
 StoreNewColor (private)

	Stores the given ColorInfo in the first available slot.

 ******************************************************************************/

JColorIndex
JXColormap::StoreNewColor
	(
	const ColorInfo& newInfo
	)
{
	assert( itsColorList != NULL );

	const JSize colorCount = itsColorList->GetElementCount();
	for (JIndex i=1; i<=colorCount; i++)
		{
		ColorInfo info = itsColorList->GetElement(i);
		if (info.useCount == 0)
			{
			itsColorList->SetElement(i, newInfo);
			return i;
			}
		}

	itsColorList->AppendElement(newInfo);
	return colorCount + 1;
}

/******************************************************************************
 CreateEmptyColormap (private)

	Creates a private Colormap and adjusts the pixel values to
	match this new map.  If we already own the colormap, calling this
	function means that the client is out of luck -- no colormap can hold
	all the requested colors -- so we return kJFalse.

	With color approximation turned on, several new pixels may map to the
	same old pixel.  We cannot do anything about this because the information
	was lost when the JXImage was created.  It cannot be recovered now.

 ******************************************************************************/

JBoolean
JXColormap::CreateEmptyColormap()
{
JIndex i;

	if (itsOwnsColormapFlag || !itsCanSwitchCmapFlag)
		{
		return kJFalse;
		}

	assert( itsColorList != NULL );

	// create a new color map with our previous entries

	const Colormap origColormap = itsXColormap;
	itsXColormap = XCreateColormap(*itsDisplay, itsDisplay->GetRootWindow(),
								   itsDisplay->GetDefaultVisual(), AllocNone);
	if (itsXColormap == origColormap)	// custom colormap not possible
		{
		return kJFalse;
		}

	itsOwnsColormapFlag = kJTrue;

	// update the pixel values -- this should only happen for PseudoColor

	unsigned long* pixelTable = new unsigned long [ GetMaxColorCount() ];
	assert( pixelTable != NULL );

	const JSize colorCount = itsColorList->GetElementCount();

	// allocate guaranteed colors

	for (i=1; i<=kLastPredefColor; i++)
		{
		ColorInfo info = itsColorList->GetElement(i);
		assert( !info.dynamic && info.useCount > 0 );

		XFreeColors(*itsDisplay, origColormap, &(info.xPixel), 1, 0);

		XColor xColor;
		xColor.red      = info.color.red;
		xColor.green    = info.color.green;
		xColor.blue     = info.color.blue;
		xColor.flags    = DoRed | DoGreen | DoBlue;
		const Status ok = XAllocColor(*itsDisplay, itsXColormap, &xColor);
		assert( ok );		// the colormap started out empty

		assert( info.xPixel < GetMaxColorCount() );
		pixelTable[ info.xPixel ] = xColor.pixel;

		info.xPixel     = xColor.pixel;
		info.exactMatch = kJTrue;
		itsColorList->SetElement(i, info);
		}

	// allocate dynamic colors first since they cannot be approximated

	for (i=kLastPredefColor+1; i<=colorCount; i++)
		{
		ColorInfo info = itsColorList->GetElement(i);
		if (info.dynamic && info.useCount > 0)
			{
			XFreeColors(*itsDisplay, origColormap, &(info.xPixel), 1, 0);

			unsigned long xPixel;
			const Status ok =
				XAllocColorCells(*itsDisplay, itsXColormap, False, NULL, 0,
								 &xPixel, 1);
			assert( ok );		// one cannot have more than the size of the colormap

			assert( info.xPixel < GetMaxColorCount() );
			pixelTable[ info.xPixel ] = xPixel;

			info.xPixel = xPixel;
			itsColorList->SetElement(i, info);
			}
		}

	// allocate rest of static colors

	const long saveHW = itsColorBoxHW;
	itsColorBoxHW     = LONG_MAX;		// guarantee color approximation

	AllocateXColorList();
	itsSwitchingCmapFlag = kJTrue;

	for (i=kLastPredefColor+1; i<=colorCount; i++)
		{
		ColorInfo origInfo = itsColorList->GetElement(i);
		if (!origInfo.dynamic && origInfo.useCount > 0)
			{
			XFreeColors(*itsDisplay, origColormap, &(origInfo.xPixel), 1, 0);

			JColorIndex colorIndex;
			const JBoolean ok = JColormap::AllocateStaticColor(origInfo.color, &colorIndex);
			assert( ok );

			ColorInfo newInfo = itsColorList->GetElement(colorIndex);

			assert( origInfo.xPixel < GetMaxColorCount() );
			pixelTable[ origInfo.xPixel ] = newInfo.xPixel;

			origInfo.xPixel     = newInfo.xPixel;
			origInfo.exactMatch = newInfo.exactMatch;
			itsColorList->SetElement(i, origInfo);

			if (colorIndex <= colorCount)
				{
				newInfo.useCount = 0;
				itsColorList->SetElement(colorIndex, newInfo);
				}
			else
				{
				assert( colorIndex == colorCount + 1 );
				itsColorList->RemoveElement(colorIndex);
				}
			}
		}

	FreeXColorList();
	itsColorBoxHW        = saveHW;
	itsSwitchingCmapFlag = kJFalse;

	// notify everybody

	if (itsNotifyNewCmapFlag)
		{
		Broadcast( NewColormap(pixelTable) );
		itsDisplay->ColormapChanged(this);
		}

	// clean up

	delete [] pixelTable;
	return kJTrue;
}

/******************************************************************************
 GetXPixel

	Returns the Pixel value that X understands.

 ******************************************************************************/

unsigned long
JXColormap::GetXPixel
	(
	const JColorIndex colorIndex
	)
	const
{
	if (itsAllColorsPreallocatedFlag)
		{
		return colorIndex;
		}
	else
		{
		return (itsColorList->GetElement(colorIndex)).xPixel;
		}
}

/******************************************************************************
 GetSystemColorIndex (virtual)

	Returns the Pixel value that X understands.

	*** Only guaranteed to work when using PseudoColor.

 ******************************************************************************/

int
JXColormap::GetSystemColorIndex
	(
	const JColorIndex colorIndex
	)
	const
{
	const unsigned long xPixel = GetXPixel(colorIndex);
	assert( xPixel < INT_MAX );
	return (int) xPixel;
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
	if (itsAllColorsPreallocatedFlag)
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
	else
		{
		const ColorInfo info = itsColorList->GetElement(colorIndex);
		*red   = info.color.red;
		*green = info.color.green;
		*blue  = info.color.blue;
		}
}

/******************************************************************************
 GetColorIndex

 ******************************************************************************/

JBoolean
JXColormap::GetColorIndex
	(
	const unsigned long	xPixel,
	JColorIndex*		colorIndex
	)
	const
{
	if (itsAllColorsPreallocatedFlag)
		{
		*colorIndex = xPixel;
		return kJTrue;
		}
	else
		{
		assert( itsColorList != NULL );

		const JSize count = itsColorList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const ColorInfo info = itsColorList->GetElement(i);
			if (info.xPixel == xPixel)
				{
				*colorIndex = i;
				return kJTrue;
				}
			}

		*colorIndex = 0;
		return kJFalse;
		}
}

#define JTemplateType JXColormap::ColorInfo
#include <JArray.tmpls>
#undef JTemplateType
