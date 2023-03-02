/******************************************************************************
 JPainter.cpp

	This class abstracts the functions required to draw to the screen and
	to a printer.  Derived classes must implement the following functions:

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
		Draw the specified rectangle.  If IsFilled() returns true,
		fill the rectangle.

	Ellipse
		Draw an ellipse inside the specified rectangle.  If IsFilled()
		returns true, fill the ellipse.

	Arc
		Draw the specified arc inside the specified rectangle.  startAngle
		follows the usual convention of zero degrees on the positive x-axis and
		increases counterclockwise.  deltaAngle tells how much of the arc
		to draw (in degrees) and is positive in the counterclockwise direction.
		If IsFilled() returns true, fill the ellipse as a pie slice.

	Polygon
		Draw the specified polygon, including the closing line from the
		last vertex to the first vertex.  top and left specify the offset of
		the vertices relative to the current origin.  If IsFilled() returns true,
		fill the polygon.

	Image
		Draw the portion of the image specified by srcRect into the
		given destRect.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JPainter.h"
#include "JFontManager.h"
#include "JColorManager.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "jGlobals.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JPainter::JPainter
	(
	JFontManager*	fontManager,
	const JRect&	defaultClipRect
	)
	:
	itsOrigin(0,0),
	itsPenLoc(0,0),
	itsLineWidth(1),
	itsFillFlag(false),
	itsClipRect(defaultClipRect),

	itsDrawDashedLinesFlag(false),
	itsDashOffset(0),
	itsDashList(nullptr),

	itsFontManager(fontManager),
	itsFont(JFontManager::GetDefaultFont()),

	itsDefClipRect(defaultClipRect),
	itsDefaultColor(JColorManager::GetBlackColor())
{
	itsPenColor = itsDefaultColor;
	itsFont.SetColor(itsDefaultColor);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPainter::~JPainter()
{
	jdelete itsDashList;
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
	itsFillFlag            = false;
	itsDrawDashedLinesFlag = false;

	itsFont = JFontManager::GetDefaultFont();
	itsFont.SetColor(itsDefaultColor);
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
	if (itsDashList == nullptr)
	{
		itsDashList = jnew JArray<JSize>(dashList);
		assert( itsDashList != nullptr );
	}
	else
	{
		*itsDashList = dashList;
	}

	itsDashOffset = dashOffset;
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

JSize
JPainter::GetLineHeight
	(
	JCoordinate* ascent,
	JCoordinate* descent
	)
	const
{
	return itsFont.GetLineHeight(itsFontManager, ascent, descent);
}

JSize
JPainter::GetLineHeight()
	const
{
	return itsFont.GetLineHeight(itsFontManager);
}

/******************************************************************************
 GetStringWidth

 ******************************************************************************/

JSize
JPainter::GetStringWidth
	(
	const JString& str
	)
	const
{
	if (str.IsEmpty())
	{
		return 0;
	}

	JSize width = 0;

	JStringIterator iter(str);
	JUtf8Character c;
	JFont f = itsFont;

	iter.BeginMatch();
	while (iter.Next(&c))
	{
		if (f.SubstituteToDisplayGlyph(itsFontManager, c))
		{
			iter.SkipPrev();
			const JStringMatch& m = iter.FinishMatch();
			if (!m.IsEmpty())
			{
				width += itsFont.GetStringWidth(itsFontManager, m.GetString());
			}

			width += f.GetCharWidth(itsFontManager, c);
			iter.SkipNext();
			iter.BeginMatch();
			f = itsFont;
		}
	}

	const JStringMatch& m = iter.FinishMatch();
	if (!m.IsEmpty())
	{
		width += itsFont.GetStringWidth(itsFontManager, m.GetString());
	}

	return width;
}

/******************************************************************************
 String

 ******************************************************************************/

void
JPainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JCoordinate	width,
	const HAlign	hAlign,
	const JCoordinate	height,
	const VAlign	vAlign
	)
{
	if (str.IsEmpty())
	{
		return;
	}

	const JFont origFont = itsFont;

	JCoordinate x = left;
	JCoordinate y = top;
	AlignString(&x,&y, str, width, hAlign, height, vAlign);

	JStringIterator iter(str);
	JUtf8Character c;
	JFont f = origFont;

	iter.BeginMatch();
	while (iter.Next(&c))
	{
		if (f.SubstituteToDisplayGlyph(itsFontManager, c))
		{
			iter.SkipPrev();
			const JStringMatch& m = iter.FinishMatch();
			if (!m.IsEmpty())
			{
				const JString& s = m.GetString();
				StringNoSubstitutions(x, y, s);
				x += itsFont.GetStringWidth(itsFontManager, s);
			}

			SetFont(f);
			JString s(c);
			StringNoSubstitutions(x, y, s);
			x += f.GetCharWidth(itsFontManager, c);

			iter.SkipNext();
			iter.BeginMatch();
			f = origFont;
			SetFont(origFont);
		}
	}

	const JStringMatch& m = iter.FinishMatch();
	if (!m.IsEmpty())
	{
		StringNoSubstitutions(x, y, m.GetString());
	}
}

/******************************************************************************
 AlignString (protected)

	Adjust the given coordinates as appropriate for the given alignment.
	Returns the width of the string, if it was computed.

 ******************************************************************************/

JSize
JPainter::AlignString
	(
	JCoordinate*		left,
	JCoordinate*		top,
	const JString&		str,
	const JCoordinate	width,
	const HAlign	hAlign,
	const JCoordinate	height,
	const VAlign	vAlign
	)
	const
{
	JSize strWidth = 0;
	if (hAlign == HAlign::kCenter)
	{
		strWidth = GetStringWidth(str);
		*left += (width-strWidth)/2;
	}
	else if (hAlign == HAlign::kRight)
	{
		strWidth = GetStringWidth(str);
		*left += width-strWidth;
	}

	if (vAlign == VAlign::kCenter)
	{
		const JCoordinate lineHeight = GetLineHeight();
		*top += (height-lineHeight)/2;
	}
	else if (vAlign == VAlign::kBottom)
	{
		const JCoordinate lineHeight = GetLineHeight();
		*top += height-lineHeight;
	}

	return strWidth;
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

/******************************************************************************
 String

 ******************************************************************************/

void
JPainter::String
	(
	const JFloat		userAngle,
	const JRect&		rect,
	const JString&		str,
	const HAlign	hAlign,
	const VAlign	vAlign
	)
{
	// adjust the angle to lie between -45 and 315

	JFloat angle = userAngle;
	while (angle <= -45.0)
	{
		angle += 360.0;
	}
	while (angle > 315.0)
	{
		angle -= 360.0;
	}

	// pick the correct origin

	JPoint topLeft;
	if (-45.0 < angle && angle <= 45.0)
	{
		topLeft = rect.topLeft();
	}
	else if (45.0 < angle && angle <= 135.0)
	{
		topLeft = rect.bottomLeft();
	}
	else if (135.0 < angle && angle <= 225.0)
	{
		topLeft = rect.bottomRight();
	}
	else	// 225.0 < angle && angle <= 315.0
	{
		topLeft = rect.topRight();
	}

	String(angle, topLeft, str, rect.width(), hAlign, rect.height(), vAlign);
}
