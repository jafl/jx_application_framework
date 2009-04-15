/******************************************************************************
 JXGC.cpp

	Represents a single X Graphics Context.  We don't provide a convertion
	to GC because that would break our buffering of GC settings. (color, etc)
	This buffering is important for reducing the number of server calls.

	If we are depth 1, drawing in any color other than JXImageMask::kPixelOff
	produces "pixel on".  Refer to JXImageMask::ColorToBit().

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXGC.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <JXImageMask.h>
#include <JXFontManager.h>
#include <jXUtil.h>
#include <JMinMax.h>
#include <jMath.h>
#include <string.h>
#include <jAssert.h>

const long kDegToXAngle = 64;

const int kDefaultCapStyle  = CapButt;
const int kDefaultJoinStyle = JoinMiter;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGC::JXGC
	(
	JXDisplay*		display,
	JXColormap*		colormap,
	const Drawable	drawable
	)
	:
	itsClipOffset(0,0)
{
	itsDisplay  = display;
	itsColormap = colormap;

	XGCValues values;
	values.graphics_exposures = False;
	itsXGC = XCreateGC(*itsDisplay, drawable, 0L, &values);

	Window rootWindow;
	int x, y;
	unsigned int w, h, bw, depth;
	const Status ok = XGetGeometry(*display, drawable, &rootWindow,
								   &x,&y, &w,&h, &bw, &depth);
	assert( ok );
	itsDepth = depth;

	itsClipRegion = NULL;
	itsClipPixmap = None;

	itsLastColorInit     = kJFalse;
	itsLastColor         = colormap->GetBlackColor();
	itsLastFunction      = GXcopy;
	itsLastLineWidth     = 0;
	itsDashedLinesFlag   = kJFalse;
	itsLastFont          = 0;
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

	Returns kJTrue if we have any clipping set.  *region is created, if
	necessary.  *pixmap is set to the current pixmap, if any.
	Thus, the caller owns the resulting region, but we own the resulting pixmap.

 ******************************************************************************/

JBoolean
JXGC::GetClipping
	(
	JPoint*			offset,
	Region*			region,
	JXImageMask**	pixmap
	)
{
	*offset = itsClipOffset;

	if (itsClipRegion != NULL)
		{
		*region = JXCopyRegion(itsClipRegion);
		}
	else
		{
		*region = NULL;
		}

	if (itsClipPixmap != None)
		{
		*pixmap = new JXImageMask(itsDisplay, itsColormap, itsClipPixmap);
		assert( *pixmap != NULL );
		}
	else
		{
		*pixmap = NULL;
		}

	return JConvertToBoolean( itsClipRegion != NULL || itsClipPixmap != None );
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
	if (itsClipRegion != NULL || itsClipPixmap != None)
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

	if (itsClipRegion != NULL)
		{
		XDestroyRegion(itsClipRegion);
		itsClipRegion = NULL;
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
	const JColorIndex color
	)
{
	if (color != itsLastColor || !itsLastColorInit)
		{
		itsLastColorInit = kJTrue;
		itsLastColor     = color;

		unsigned long xPixel;
		if (itsDepth == 1)
			{
			xPixel = JXImageMask::ColorToBit(color);
			}
		else
			{
			xPixel = itsColormap->GetXPixel(color);
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
	const JBoolean on
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
	const JSize dashCount = dashList.GetElementCount();

	char* xDashList = new char [ dashCount ];
	assert( xDashList != NULL );

	for (JIndex i=1; i<=dashCount; i++)
		{
		xDashList[i-1] = dashList.GetElement(i);
		}

	XSetDashes(*itsDisplay, itsXGC, offset, xDashList, dashCount);

	delete [] xDashList;
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
 SetFont

 ******************************************************************************/

void
JXGC::SetFont
	(
	const JFontID id
	)
{
	if (id != itsLastFont)
		{
		itsLastFont = id;

		XFontStruct* xfont = (itsDisplay->GetXFontManager())->GetXFontInfo(id);
		XSetFont(*itsDisplay, itsXGC, xfont->fid);
		}
}

/******************************************************************************
 DrawString

	If the string is too long to be sent to the server in one chunk,
	we split it up.

 ******************************************************************************/

void
JXGC::DrawString
	(
	const Drawable		drawable,
	const JCoordinate	origX,
	const JCoordinate	y,
	const JCharacter*	str
	)
	const
{
	const JFontManager* fontMgr = itsDisplay->GetFontManager();

	const JSize length          = strlen(str);
	const JSize maxStringLength = itsDisplay->GetMaxStringLength();

	JCoordinate x = origX;
	JSize offset  = 0;
	while (offset < length)
		{
		const JSize count = JMin(length - offset, maxStringLength);
		XDrawString(*itsDisplay, drawable, itsXGC, x,y, str + offset, count);

		if (offset + count < length)
			{
			x += fontMgr->GetStringWidth(itsLastFont, 0, JFontStyle(),
										 str + offset, count);
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
