/******************************************************************************
 JXGC.cpp

	Represents a single X Graphics Context.  We don't provide a convertion
	to GC because that would break our buffering of GC settings. (color, etc)
	This buffering is important for reducing the number of server calls.

	If we are depth 1, drawing in any color other than JXImageMask::kPixelOff
	produces "pixel on".  Refer to JXImageMask::ColorToBit().

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include "JXGC.h"
#include "JXDisplay.h"
#include "JXColorManager.h"
#include "JXImageMask.h"
#include "JXFontManager.h"
#include "jXUtil.h"
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jMath.h>
#include <string.h>
#include <jx-af/jcore/jAssert.h>

const long kDegToXAngle = 64;

const int kDefaultCapStyle  = CapButt;
const int kDefaultJoinStyle = JoinMiter;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGC::JXGC
	(
	JXDisplay*		display,
	const Drawable	drawable
	)
	:
	itsDisplay(display),
	itsClipOffset(0,0)
{
	XGCValues values;
	values.graphics_exposures = False;
	itsXGC = XCreateGC(*itsDisplay, drawable, GCGraphicsExposures, &values);

	Window rootWindow;
	int x, y;
	unsigned int w, h, bw, depth;
	const Status ok = XGetGeometry(*display, drawable, &rootWindow,
								   &x,&y, &w,&h, &bw, &depth);
	assert( ok );
	itsDepth = depth;

	itsClipRegion = nullptr;
	itsClipPixmap = None;

	itsLastColorInit     = false;
	itsLastColor         = JColorManager::GetBlackColor();
	itsLastFunction      = GXcopy;
	itsLastLineWidth     = 0;
	itsDashedLinesFlag   = false;
	itsLastFontID        = 0;
	itsLastSubwindowMode = ClipByChildren;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGC::~JXGC()
{
	ClearPrivateClipping();
	XFreeGC(*itsDisplay, itsXGC);
}

/******************************************************************************
 GetClipping

	Returns true if we have any clipping set.  *region is created, if
	necessary.  *pixmap is set to the current pixmap, if any.
	Thus, the caller owns the resulting region, but we own the resulting pixmap.

 ******************************************************************************/

bool
JXGC::GetClipping
	(
	JPoint*			offset,
	Region*			region,
	JXImageMask**	pixmap
	)
{
	*offset = itsClipOffset;

	if (itsClipRegion != nullptr)
	{
		*region = JXCopyRegion(itsClipRegion);
	}
	else
	{
		*region = nullptr;
	}

	if (itsClipPixmap != None)
	{
		*pixmap = jnew JXImageMask(itsDisplay, itsClipPixmap);
	}
	else
	{
		*pixmap = nullptr;
	}

	return itsClipRegion != nullptr || itsClipPixmap != None;
}

/******************************************************************************
 SetClipRect

 ******************************************************************************/

void
JXGC::SetClipRect
	(
	const JRect& clipRect
	)
{
	ClearPrivateClipping();

	XRectangle xrect = JXJToXRect(clipRect);
	itsClipRegion    = JXRectangleRegion(&xrect);
	itsClipOffset    = JPoint(0,0);
	XSetClipRectangles(*itsDisplay, itsXGC, 0,0, &xrect, 1, Unsorted);
}

/******************************************************************************
 SetClipRegion

	We do not take ownership of the clipRegion that is passed in.

 ******************************************************************************/

void
JXGC::SetClipRegion
	(
	const Region clipRegion
	)
{
	ClearPrivateClipping();

	itsClipRegion = JXCopyRegion(clipRegion);
	itsClipOffset = JPoint(0,0);
	XSetRegion(*itsDisplay, itsXGC, clipRegion);

	const unsigned long valueMask = GCClipXOrigin | GCClipYOrigin;
	XGCValues values;
	values.clip_x_origin = 0;
	values.clip_y_origin = 0;
	XChangeGC(*itsDisplay, itsXGC, valueMask, &values);
}

/******************************************************************************
 SetClipPixmap

	We take ownership of the pixmap that is passed in.

 ******************************************************************************/

void
JXGC::SetClipPixmap
	(
	const JPoint&	offset,
	Pixmap			pixmap
	)
{
	ClearPrivateClipping();

	itsClipPixmap = pixmap;
	itsClipOffset = offset;
	XSetClipMask(*itsDisplay, itsXGC, pixmap);

	const unsigned long valueMask = GCClipXOrigin | GCClipYOrigin;
	XGCValues values;
	values.clip_x_origin = offset.x;
	values.clip_y_origin = offset.y;
	XChangeGC(*itsDisplay, itsXGC, valueMask, &values);
}

/******************************************************************************
 SetClipPixmap

 ******************************************************************************/

void
JXGC::SetClipPixmap
	(
	const JPoint&		offset,
	const JXImageMask&	mask
	)
{
	SetClipPixmap(offset, mask.CreatePixmap());
}

/******************************************************************************
 ClearClipping

 ******************************************************************************/

void
JXGC::ClearClipping()
{
	if (itsClipRegion != nullptr || itsClipPixmap != None)
	{
		ClearPrivateClipping();
		XSetClipMask(*itsDisplay, itsXGC, None);
	}
}

/******************************************************************************
 ClearPrivateClipping (private)

 ******************************************************************************/

void
JXGC::ClearPrivateClipping()
{
	itsClipOffset = JPoint(0,0);

	if (itsClipRegion != nullptr)
	{
		XDestroyRegion(itsClipRegion);
		itsClipRegion = nullptr;
	}

	if (itsClipPixmap != None)
	{
		XFreePixmap(*itsDisplay, itsClipPixmap);
		itsClipPixmap = None;
	}
}

/******************************************************************************
 SetDrawingColor

 ******************************************************************************/

void
JXGC::SetDrawingColor
	(
	const JColorID color
	)
{
	if (color != itsLastColor || !itsLastColorInit)
	{
		itsLastColorInit = true;
		itsLastColor     = color;

		unsigned long xPixel;
		if (itsDepth == 1)
		{
			xPixel = JXImageMask::ColorToBit(color);
		}
		else
		{
			xPixel = itsDisplay->GetColorManager()->GetXColor(color);
		}

		XSetForeground(*itsDisplay, itsXGC, xPixel);
	}
}

/******************************************************************************
 SetDrawingFunction

 ******************************************************************************/

void
JXGC::SetDrawingFunction
	(
	const int function
	)
{
	if (function != itsLastFunction)
	{
		itsLastFunction = function;
		XSetFunction(*itsDisplay, itsXGC, function);
	}
}

/******************************************************************************
 SetLineWidth

 ******************************************************************************/

void
JXGC::SetLineWidth
	(
	const JSize width
	)
{
	if (width != itsLastLineWidth)
	{
		itsLastLineWidth = width;

		XSetLineAttributes(*itsDisplay, itsXGC,
						   itsLastLineWidth, GetXLineStyle(itsDashedLinesFlag),
						   kDefaultCapStyle, kDefaultJoinStyle);
	}
}

/******************************************************************************
 DrawDashedLines

 ******************************************************************************/

void
JXGC::DrawDashedLines
	(
	const bool on
	)
{
	if (on != itsDashedLinesFlag)
	{
		itsDashedLinesFlag = on;

		XSetLineAttributes(*itsDisplay, itsXGC,
						   itsLastLineWidth, GetXLineStyle(itsDashedLinesFlag),
						   kDefaultCapStyle, kDefaultJoinStyle);
	}
}

/******************************************************************************
 SetDashList

 ******************************************************************************/

void
JXGC::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				offset
	)
{
	const JSize dashCount = dashList.GetItemCount();

	char* xDashList = jnew char [ dashCount ];
	for (JIndex i=1; i<=dashCount; i++)
	{
		xDashList[i-1] = dashList.GetItem(i);
	}

	XSetDashes(*itsDisplay, itsXGC, offset, xDashList, dashCount);

	jdelete [] xDashList;
}

/******************************************************************************
 GetXLineStyle (private)

 ******************************************************************************/

inline int
JXGC::GetXLineStyle
	(
	const bool drawDashedLines
	)
	const
{
	return (drawDashedLines ? LineOnOffDash : LineSolid);
}

/******************************************************************************
 SetSubwindowMode

	Possible arguments are: ClipByChildren, IncludeInferiors

 ******************************************************************************/

void
JXGC::SetSubwindowMode
	(
	const int mode
	)
{
	if (mode != itsLastSubwindowMode)
	{
		itsLastSubwindowMode = mode;
		XSetSubwindowMode(*itsDisplay, itsXGC, mode);
	}
}

/******************************************************************************
 DrawPoint

 ******************************************************************************/

void
JXGC::DrawPoint
	(
	const Drawable		drawable,
	const JCoordinate	x,
	const JCoordinate	y
	)
	const
{
	XDrawPoint(*itsDisplay, drawable, itsXGC, x,y);
}

/******************************************************************************
 DrawLine

 ******************************************************************************/

void
JXGC::DrawLine
	(
	const Drawable		drawable,
	const JCoordinate	x1,
	const JCoordinate	y1,
	const JCoordinate	x2,
	const JCoordinate	y2
	)
	const
{
	XDrawLine(*itsDisplay, drawable, itsXGC, x1,y1, x2,y2);

	if (itsLastLineWidth == 1)		// X sometimes doesn't draw either end point
	{
		XPoint pt[2];	// initialization with { ... } causes core dump
		pt[0].x = x1;
		pt[0].y = y1;
		pt[1].x = x2;
		pt[1].y = y2;
		XDrawPoints(*itsDisplay, drawable, itsXGC, pt, 2, CoordModeOrigin);
	}
}

/******************************************************************************
 DrawLines

	xpt[0] must be the same as xpt[ptCount-1] in order to close the polygon.

 ******************************************************************************/

void
JXGC::DrawLines
	(
	const Drawable	drawable,
	const JSize		ptCount,
	XPoint			xpt[]
	)
	const
{
	const JSize kMaxPointCount = 1 + (XMaxRequestSize(*itsDisplay) - 3) / 2;

	JSize offset = 0;
	while (offset < ptCount-1)
	{
		const JSize count = JMin(ptCount - offset, kMaxPointCount);
		XDrawLines(*itsDisplay, drawable, itsXGC, &(xpt[offset]), count, CoordModeOrigin);
		offset += count - 1;
	}
}

/******************************************************************************
 DrawRect

 ******************************************************************************/

void
JXGC::DrawRect
	(
	const Drawable		drawable,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	width,
	const JCoordinate	height
	)
	const
{
	XDrawRectangle(*itsDisplay, drawable, itsXGC, x,y, width-1,height-1);
}

/******************************************************************************
 FillRect

 ******************************************************************************/

void
JXGC::FillRect
	(
	const Drawable		drawable,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	width,
	const JCoordinate	height
	)
	const
{
	XFillRectangle(*itsDisplay, drawable, itsXGC, x,y, width,height);
}

/******************************************************************************
 DrawArc

 ******************************************************************************/

void
JXGC::DrawArc
	(
	const Drawable		drawable,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	width,
	const JCoordinate	height,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
	const
{
	XDrawArc(*itsDisplay, drawable, itsXGC, x,y, width-1,height-1,
			 JRound(startAngle*kDegToXAngle),
			 JRound(deltaAngle*kDegToXAngle));
}

/******************************************************************************
 FillArc

 ******************************************************************************/

void
JXGC::FillArc
	(
	const Drawable		drawable,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	width,
	const JCoordinate	height,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
	const
{
	XFillArc(*itsDisplay, drawable, itsXGC, x,y, width-1,height-1,
			 JRound(startAngle*kDegToXAngle),
			 JRound(deltaAngle*kDegToXAngle));
}

/******************************************************************************
 FillPolygon

 ******************************************************************************/

void
JXGC::FillPolygon
	(
	const Drawable	drawable,
	const JSize		ptCount,
	XPoint			xpt[]
	)
	const
{
	XFillPolygon(*itsDisplay, drawable, itsXGC, xpt, ptCount,
				 Complex, CoordModeOrigin);
}

/******************************************************************************
 DrawString

	If the string is too long to be sent to the server in one chunk,
	we split it up.

 ******************************************************************************/

void
JXGC::DrawString
	(
	Drawable			drawable,
	XftDraw*			fdrawable,
	const JCoordinate	origX,
	const JCoordinate	y,
	const JString&		str
	)
	const
{
	const JXFontManager::XFont xfont = itsDisplay->GetXFontManager()->GetXFontInfo(itsLastFontID);

	XftColor color;
	if (xfont.type == JXFontManager::kTrueType)
	{
		const JRGB rgb = JColorManager::GetRGB(itsLastColor);

		XRenderColor renderColor;
		renderColor.red   = rgb.red;
		renderColor.green = rgb.green;
		renderColor.blue  = rgb.blue;
		renderColor.alpha = 65535;

		XftColorAllocValue(*itsDisplay, itsDisplay->GetDefaultVisual(),
						   itsDisplay->GetColorManager()->GetXColormap(),
						   &renderColor, &color);
	}

	const JSize byteCount      = str.GetByteCount();
	const JSize chunkByteCount = itsDisplay->GetMaxStringByteCount();

	JCoordinate x = origX;
	JSize offset  = 0;
	while (offset < byteCount)
	{
		const JUtf8Byte* s = str.GetRawBytes() + offset;	// GetRawBytes() because str may be a shadow

		JSize count = byteCount - offset;
		if (count > chunkByteCount)
		{
			count = chunkByteCount;
			JSize byteCount;
			while (!JUtf8Character::GetCharacterByteCount(s + count, &byteCount))
			{
				count--;
			}
		}

		if (xfont.type == JXFontManager::kStdType)
		{
			// Xutf8DrawString() would be more accurate, but it's not standard, and XmbDrawString() supposedly does a better job.
			XmbDrawString(*itsDisplay, drawable, xfont.xfset, itsXGC, x,y, s, count);
		}
		else
		{
			assert( xfont.type == JXFontManager::kTrueType );
			XftDrawStringUtf8(fdrawable, &color, xfont.xftt, x,y, (FcChar8*) s, count);
		}

		if (offset + count < byteCount)
		{
			x += itsDisplay->GetXFontManager()->GetStringWidth(itsLastFontID, JString(s, count, JString::kNoCopy));
		}
		offset += count;
	}
}

/******************************************************************************
 CopyPixels

 ******************************************************************************/

void
JXGC::CopyPixels
	(
	const Drawable		source,
	const JCoordinate	src_x,
	const JCoordinate	src_y,
	const JCoordinate	width,
	const JCoordinate	height,
	const Drawable		dest,
	const JCoordinate	dest_x,
	const JCoordinate	dest_y
	)
	const
{
	XCopyArea(*itsDisplay, source, dest, itsXGC,
			  src_x, src_y, width, height, dest_x, dest_y);
}

/******************************************************************************
 CopyImage

 ******************************************************************************/

void
JXGC::CopyImage
	(
	const XImage*		source,
	const JCoordinate	src_x,
	const JCoordinate	src_y,
	const JCoordinate	width,
	const JCoordinate	height,
	const Drawable		dest,
	const JCoordinate	dest_x,
	const JCoordinate	dest_y
	)
	const
{
	XPutImage(*itsDisplay, dest, itsXGC, const_cast<XImage*>(source),
			  src_x, src_y, dest_x, dest_y, width, height);
}
