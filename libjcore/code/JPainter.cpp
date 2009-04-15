/******************************************************************************
 JPainter.cpp

	This class abstracts the functions required to draw to the screen and
	to a printer.  Derived classes must implement the following routines:

	SetClipRect
		Set the rectangle that drawing will be clipped to.

	String
		The first version draws text horizontally inside the specified
		rectangle with the specified horizontal and vertical alignment.
		The second version draws rotated text.  This routine is only
		required to handle rotations of 0, 90, 180, and 270 degrees.

	Point
		Draw a point.

	Line
		Draw a line between the two specified points.  Call SetPenLocation()
		with the ending point.

	Rect
		Draw the specified rectangle.  If IsFilled() returns kJTrue,
		fill the rectangle.

	Ellipse
		Draw an ellipse inside the specified rectangle.  If IsFilled()
		returns kJTrue, fill the ellipse.

	Arc
		Draw the specified arc inside the specified rectangle.  startAngle
		follows the usual convention of zero degrees on the positive x-axis and
		increases counterclockwise.  deltaAngle tells how much of the arc
		to draw (in degrees) and is positive in the counterclockwise direction.
		If IsFilled() returns kJTrue, fill the ellipse as a pie slice.

	Polygon
		Draw the specified polygon, including the closing line from the
		last vertex to the first vertex.  top and left specify the offset of
		the vertices relative to the current origin.  If IsFilled() returns kJTrue,
		fill the polygon.

	Image
		Draw the portion of the image specified by srcRect into the
		given destRect.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JPainter::JPainter
	(
	const JFontManager*	fontManager,
	const JColormap*	colormap,
	const JRect&		defaultClipRect
	)
	:
	itsOrigin(0,0),
	itsPenLoc(0,0),
	itsLineWidth(1),
	itsFillFlag(kJFalse),
	itsClipRect(defaultClipRect),

	itsDrawDashedLinesFlag(kJFalse),
	itsDashOffset(0),
	itsDashList(NULL),

	itsFontManager(fontManager),
	itsColormap(colormap),

	itsFontSize(kJDefaultFontSize),

	itsDefClipRect(defaultClipRect),
	itsDefaultColor(itsColormap->GetBlackColor())
{
	itsPenColor  = itsDefaultColor;
	itsFontStyle = JFontStyle(itsDefaultColor);

	itsFontName = new JString(JGetDefaultFontName());
	assert( itsFontName != NULL );

	itsFontID = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPainter::~JPainter()
{
	delete itsDashList;
	delete itsFontName;
}

/******************************************************************************
 Reset (virtual)

	Reset parameters that may have been changed and that we have reasonable
	defaults for.

	The second version accepts a default clip rect.

 ******************************************************************************/

void
JPainter::Reset()
{
	ResetAllButClipping();
	ResetClipRect();
}

void
JPainter::Reset
	(
	const JRect& defClipRect
	)
{
	itsDefClipRect = defClipRect;
	Reset();
}

void
JPainter::ResetAllButClipping()
{
	itsPenLoc              = itsOrigin;
	itsLineWidth           = 1;
	itsPenColor            = itsDefaultColor;
	itsFillFlag            = kJFalse;
	itsDrawDashedLinesFlag = kJFalse;

	*itsFontName = JGetDefaultFontName();
	itsFontSize  = kJDefaultFontSize;
	itsFontStyle = JFontStyle(itsDefaultColor);

	itsFontID = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
}

/******************************************************************************
 SetClipRect (virtual)

 ******************************************************************************/

JRect
JPainter::SetClipRect
	(
	const JRect& r
	)
{
	JRect localDefClipRect = itsDefClipRect;
	localDefClipRect.Shift(-itsOrigin);			// to local
	JIntersection(r, localDefClipRect, &itsClipRect);
	return itsClipRect;
}

/******************************************************************************
 ResetClipRect

 ******************************************************************************/

void
JPainter::ResetClipRect()
{
	JRect localDefClipRect = itsDefClipRect;
	localDefClipRect.Shift(-itsOrigin);		// to local
	SetClipRect(localDefClipRect);			// always call since it can be overridden
}

/******************************************************************************
 SetDefaultClipRect

 ******************************************************************************/

void
JPainter::SetDefaultClipRect
	(
	const JRect& r
	)
{
	itsDefClipRect = r;

	const JRect r1 = itsClipRect;	// must make copy since pass by reference
	SetClipRect(r1);
}

/******************************************************************************
 SetLineWidth

 ******************************************************************************/

void
JPainter::SetLineWidth
	(
	const JSize width
	)
{
	assert( width > 0 );
	itsLineWidth = width;
}

/******************************************************************************
 SetDashList (virtual)

 ******************************************************************************/

void
JPainter::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	if (itsDashList == NULL)
		{
		itsDashList = new JArray<JSize>(dashList);
		assert( itsDashList != NULL );
		}
	else
		{
		*itsDashList = dashList;
		}

	itsDashOffset = dashOffset;
}

/******************************************************************************
 Set font info

 ******************************************************************************/

void
JPainter::SetFontName
	(
	const JCharacter* name
	)
{
	if (name != *itsFontName)
		{
		*itsFontName = name;
		itsFontID    = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
		}
}

void
JPainter::SetFontID
	(
	const JFontID id
	)
{
	if (id != itsFontID)
		{
		itsFontID    = id;
		*itsFontName = itsFontManager->GetFontName(itsFontID);
		}
}

void
JPainter::SetFontSize
	(
	const JSize size
	)
{
	if (size != itsFontSize)
		{
		itsFontSize = size;
		itsFontID   = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
		}
}

void
JPainter::SetFontStyle
	(
	const JFontStyle& style
	)
{
	if (style != itsFontStyle)
		{
		itsFontStyle = style;
		itsFontID    = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
		}
}

void
JPainter::SetFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	*itsFontName = name;
	itsFontSize  = size;
	itsFontStyle = style;
	itsFontID    = itsFontManager->GetFontID(*itsFontName, itsFontSize, itsFontStyle);
}

void
JPainter::SetFont
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style
	)
{
	itsFontID    = id;
	itsFontSize  = size;
	itsFontStyle = style;
	*itsFontName = itsFontManager->GetFontName(itsFontID);
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

JSize
JPainter::GetLineHeight
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
	return itsFontManager->GetLineHeight(name, size, style, ascent, descent);
}

JSize
JPainter::GetLineHeight
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	return itsFontManager->GetLineHeight(name, size, style);
}

JSize
JPainter::GetLineHeight
	(
	JCoordinate* ascent,
	JCoordinate* descent
	)
	const
{
	return itsFontManager->GetLineHeight(itsFontID, itsFontSize, itsFontStyle, ascent, descent);
}

JSize
JPainter::GetLineHeight()
	const
{
	return itsFontManager->GetLineHeight(itsFontID, itsFontSize, itsFontStyle);
}

/******************************************************************************
 GetStringWidth

 ******************************************************************************/

JSize
JPainter::GetStringWidth
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str
	)
	const
{
	return itsFontManager->GetStringWidth(name, size, style, str);
}

JSize
JPainter::GetStringWidth
	(
	const JCharacter* str
	)
	const
{
	return itsFontManager->GetStringWidth(itsFontID, itsFontSize, itsFontStyle, str);
}

/******************************************************************************
 AlignString (protected)

	Adjust the given coordinates as appropriate for the given alignment.

 ******************************************************************************/

void
JPainter::AlignString
	(
	JCoordinate*		left,
	JCoordinate*		top,
	const JCharacter*	str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
	const
{
	if (hAlign == kHAlignCenter)
		{
		const JSize strWidth = GetStringWidth(str);
		*left += (width-strWidth)/2;
		}
	else if (hAlign == kHAlignRight)
		{
		const JSize strWidth = GetStringWidth(str);
		*left += width-strWidth;
		}

	if (vAlign == kVAlignCenter)
		{
		const JSize lineHeight = GetLineHeight();
		*top += (height-lineHeight)/2;
		}
	else if (vAlign == kVAlignBottom)
		{
		const JSize lineHeight = GetLineHeight();
		*top += height-lineHeight;
		}
}

/******************************************************************************
 RectInside

	Draws a rectangle inside the given frame, instead of centered on it.

 ******************************************************************************/

void
JPainter::RectInside
	(
	const JRect& bounds
	)
{
	JRect r   = bounds;		// adjust for X centering thickness around line
	r.top    += itsLineWidth/2;
	r.left   += itsLineWidth/2;
	r.bottom -= (itsLineWidth-1)/2;
	r.right  -= (itsLineWidth-1)/2;

	Rect(r);
}
