/******************************************************************************
 JXWindowPainter.cpp

	This class implements the functions required to draw to an X window.

	We don't provide functions to manipulate the default clip region because
	this is reserved for JXWindow, which constructs us.

	BASE CLASS = JPainter

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXWindowPainter.h"
#include "JXDisplay.h"
#include "JXGC.h"
#include "JXColorManager.h"
#include "JXImage.h"
#include "JXImagePainter.h"
#include "jXUtil.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	defaultClipRegion can be nullptr

 ******************************************************************************/

JXWindowPainter::JXWindowPainter
	(
	JXGC*			gc,
	Drawable		drawable,
	const JRect&	defaultClipRect,
	const Region	defaultClipRegion
	)
	:
	JPainter((gc->GetDisplay())->GetFontManager(), defaultClipRect)
{
	assert( gc != nullptr );

	itsDisplay        = gc->GetDisplay();
	itsDrawable       = drawable;
	itsGC             = gc;
	itsFontDrawable   = nullptr;
	itsFontClipRegion = nullptr;

	if (defaultClipRegion != nullptr)
	{
		itsDefClipRegion = JXCopyRegion(defaultClipRegion);
		itsClipRegion    = JXCopyRegion(defaultClipRegion);
	}
	else
	{
		itsDefClipRegion = nullptr;
		itsClipRegion    = nullptr;
	}

	itsResetShouldClearClipRegionFlag = true;

	ResetClipRect();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindowPainter::~JXWindowPainter()
{
	if (itsClipRegion != nullptr)
	{
		XDestroyRegion(itsClipRegion);
	}
	if (itsDefClipRegion != nullptr)
	{
		XDestroyRegion(itsDefClipRegion);
	}
	if (itsFontDrawable != nullptr)
	{
		XftDrawDestroy(itsFontDrawable);
	}
	if (itsFontClipRegion != nullptr)
	{
		XDestroyRegion(itsFontClipRegion);
	}
}

/******************************************************************************
 GetXColorManager

 ******************************************************************************/

JXColorManager*
JXWindowPainter::GetXColorManager()
	const
{
	return itsDisplay->GetColorManager();
}

/******************************************************************************
 GetFontDrawable

 ******************************************************************************/

XftDraw*
JXWindowPainter::GetFontDrawable()
	const
{
	if (itsFontDrawable == nullptr)
	{
		const_cast<JXWindowPainter*>(this)->itsFontDrawable =
			XftDrawCreate(*itsDisplay, itsDrawable,
						  itsDisplay->GetDefaultVisual(),
						  *GetXColorManager());
		XftDrawSetClip(itsFontDrawable, itsFontClipRegion);
	}

	return itsFontDrawable;
}

/******************************************************************************
 Reset (virtual)

	Reset parameters that may have been changed and that we have reasonable
	defaults for.

	The second version accepts a default clip rect and a (non-default) clip region.
	This is mainly for use by JXContainer::DrawAll().

 ******************************************************************************/

void
JXWindowPainter::Reset()
{
	if (itsResetShouldClearClipRegionFlag)
	{
		if (itsClipRegion != nullptr)
		{
			XDestroyRegion(itsClipRegion);
			itsClipRegion = nullptr;
		}

		if (itsDefClipRegion != nullptr)
		{
			itsClipRegion = JXCopyRegion(itsDefClipRegion);
		}
	}

	// call this last so new clip region is used when ResetClipRect() is called

	JPainter::Reset();
}

void
JXWindowPainter::Reset
	(
	const JRect&	defClipRect,
	const Region	clipRegion
	)
{
	CalcClipRegion(clipRegion);
	itsResetShouldClearClipRegionFlag = false;
	JPainter::Reset(defClipRect);
	itsResetShouldClearClipRegionFlag = true;
}

/******************************************************************************
 SetClipRegion

	Must be in global coordinates.

 ******************************************************************************/

void
JXWindowPainter::SetClipRegion
	(
	const Region region
	)
{
	CalcClipRegion(region);

	// update the clipping of GC
	// (must make copy of clipRect since everything is by reference)

	const JRect clipRect = GetClipRect();
	SetClipRect(clipRect);
}

/******************************************************************************
 SetDefaultClipRegion

	Must be in global coordinates.

 ******************************************************************************/

void
JXWindowPainter::SetDefaultClipRegion
	(
	const Region region
	)
{
	if (itsDefClipRegion != nullptr)
	{
		XDestroyRegion(itsDefClipRegion);
		itsDefClipRegion = nullptr;
	}

	if (region != nullptr)
	{
		itsDefClipRegion = JXCopyRegion(region);
	}
}

/******************************************************************************
 CalcClipRegion (private)

	Sets itsClipRegion to the intersection of itsDefClipRegion and
	the given region.  Caller is required to call SetClipRect() to update
	the X GC.

 ******************************************************************************/

void
JXWindowPainter::CalcClipRegion
	(
	const Region region
	)
{
	if (itsClipRegion != nullptr)
	{
		XDestroyRegion(itsClipRegion);
	}

	if (itsDefClipRegion != nullptr)
	{
		itsClipRegion = XCreateRegion();
		XIntersectRegion(region, itsDefClipRegion, itsClipRegion);
	}
	else
	{
		itsClipRegion = JXCopyRegion(region);
	}
}

/******************************************************************************
 ResetClipRegion

 ******************************************************************************/

void
JXWindowPainter::ResetClipRegion()
{
	if (itsClipRegion != nullptr)
	{
		XDestroyRegion(itsClipRegion);
	}

	if (itsDefClipRegion != nullptr)
	{
		itsClipRegion = JXCopyRegion(itsDefClipRegion);
	}
	else
	{
		itsClipRegion = nullptr;
	}

	// update the clipping of GC
	// (must make copy of clipRect since everything is by reference)

	const JRect clipRect = GetClipRect();
	SetClipRect(clipRect);
}

/******************************************************************************
 SetClipRect (virtual)

 ******************************************************************************/

JRect
JXWindowPainter::SetClipRect
	(
	const JRect& userRect
	)
{
	if (itsFontClipRegion != nullptr)
	{
		XDestroyRegion(itsFontClipRegion);
		itsFontClipRegion = nullptr;
	}

	const JRect r    = JPainter::SetClipRect(userRect);
	const bool empty = r.IsEmpty();
	if (!empty)
	{
		JRect rG = r;
		rG.Shift(GetOrigin());		// to global

		if (itsClipRegion != nullptr)
		{
			XRectangle xrect  = JXJToXRect(rG);
			Region clipRegion = XCreateRegion();
			JXIntersectRectWithRegion(&xrect, itsClipRegion, clipRegion);
			itsGC->SetClipRegion(clipRegion);
			itsFontClipRegion = JXCopyRegion(clipRegion);
			XDestroyRegion(clipRegion);
		}
		else
		{
			itsGC->SetClipRect(rG);
			itsFontClipRegion = JXRectangleRegion(rG);
		}
	}
	else
	{
		itsGC->SetClipRect(r);
		itsFontClipRegion = JXRectangleRegion(r);
	}

	if (itsFontDrawable != nullptr)
	{
		XftDrawSetClip(itsFontDrawable, itsFontClipRegion);
	}

	return r;
}

/******************************************************************************
 SetDashList (virtual)

 ******************************************************************************/

void
JXWindowPainter::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	JPainter::SetDashList(dashList, dashOffset);
	itsGC->SetDashList(dashList, dashOffset);
}

/******************************************************************************
 String (virtual)

 ******************************************************************************/

void
JXWindowPainter::StringNoSubstitutions
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str
	)
{
	if (str.IsEmpty())
	{
		return;
	}

	const JFont& font = GetFont();
	itsGC->SetFont(font.GetID());
	itsGC->SetDrawingColor(font.GetStyle().color);

	JCoordinate ascent, descent;
	GetLineHeight(&ascent, &descent);

	const JPoint& o = GetOrigin();

	itsGC->DrawString(itsDrawable, GetFontDrawable(), o.x + left, o.y + top + ascent, str);
	StyleString(str, left, top + ascent, ascent, descent);
}

void
JXWindowPainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JIndex		uIndex,
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

	JCoordinate x = left;
	JCoordinate y = top;
	AlignString(&x,&y, str, width, hAlign, height, vAlign);

	JPainter::String(x, y, str);

	if (uIndex > 0)
	{
		const JPoint& o = GetOrigin();

		JCoordinate ascent, descent;
		GetLineHeight(&ascent, &descent);

		x += o.x;
		y += o.y + ascent;

		JStringIterator iter(str);
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartBeforeChar, uIndex);

		if (uIndex > 1)
		{
			x += GetStringWidth(iter.FinishMatch().GetString());
		}

		JUtf8Character c;
		iter.Next(&c);
		iter.Invalidate();

		JFont font = GetFont();
		font.SubstituteToDisplayGlyph(GetFontManager(), c);
		const JCoordinate w = font.GetCharWidth(GetFontManager(), c);

		const JCoordinate lineWidth = font.GetUnderlineThickness();
		y += JLFloor(1.5 * lineWidth);

		itsGC->SetLineWidth(lineWidth);
		itsGC->DrawDashedLines(false);
		itsGC->DrawLine(itsDrawable, x, y, x+w-1, y);
	}
}

void
JXWindowPainter::String
	(
	const JFloat		userAngle,
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

	// if the angle is zero, we can do it easily

	if (-45.0 < angle && angle <= 45.0)
	{
		JPainter::String(left, top, str, width, hAlign, height, vAlign);
		return;
	}

	// we have to do it pixel by pixel

	JXImage* srcImage = nullptr;
	const JPoint& o   = GetOrigin();

	JCoordinate ascent, descent;
	const JSize lineHeight = GetLineHeight(&ascent, &descent) + 1;
	JSize stringWidth;

	const JCoordinate x0 = o.x + left;
	const JCoordinate y0 = o.y + top;

	JIndex quadrant;
	JCoordinate dx=0, dy=0;
	if (45.0 < angle && angle <= 135.0)
	{
		quadrant    = 2;
		stringWidth = AlignString(&dx,&dy, str, height, hAlign, width, vAlign);
		if (stringWidth == 0)
		{
			stringWidth = GetStringWidth(str);
		}
		srcImage = jnew JXImage(itsDisplay, itsDrawable,
							   JRect(y0-dx - stringWidth, x0+dy, y0-dx + 1, x0+dy + lineHeight + 1));
	}
	else if (135.0 < angle && angle <= 225.0)
	{
		quadrant    = 3;
		stringWidth = AlignString(&dx,&dy, str, width, hAlign, height, vAlign);
		if (stringWidth == 0)
		{
			stringWidth = GetStringWidth(str);
		}
		srcImage = jnew JXImage(itsDisplay, itsDrawable,
							   JRect(y0-dy - lineHeight, x0-dx - stringWidth, y0-dy + 1, x0-dx + 1));
	}
	else	// 225.0 < angle && angle <= 315.0
	{
		quadrant    = 4;
		stringWidth = AlignString(&dx,&dy, str, height, hAlign, width, vAlign);
		if (stringWidth == 0)
		{
			stringWidth = GetStringWidth(str);
		}
		srcImage = jnew JXImage(itsDisplay, itsDrawable,
							   JRect(y0+dx, x0-dy - lineHeight, y0+dx + stringWidth + 1, x0-dy + 1));
	}
	assert( srcImage != nullptr );

	auto* tempImage = jnew JXImage(itsDisplay, stringWidth, lineHeight, 0, 0,
									 JXImage::kLocalStorage);
	assert( tempImage != nullptr );

	// transfer the source

	for (JCoordinate x=0; x < (JCoordinate) stringWidth; x++)
	{
		for (JCoordinate y=0; y < (JCoordinate) lineHeight; y++)
		{
			unsigned long srcPixel;
			if (quadrant == 2)
			{
				srcPixel = srcImage->GetColor(y, stringWidth - x);
			}
			else if (quadrant == 3)
			{
				srcPixel = srcImage->GetColor(stringWidth - x, lineHeight - y);
			}
			else	// quadrant == 4
			{
				srcPixel = srcImage->GetColor(lineHeight - y, x);
			}

			tempImage->SetColor(x,y, srcPixel);
		}
	}

	// draw the string
{
	JXImagePainter* p = tempImage->CreatePainter();
	p->SetFont(GetFont());
	p->JPainter::String(0,0, str);
	jdelete p;
}

	// transfer the result

	for (JCoordinate x=0; x < (JCoordinate) stringWidth; x++)
	{
		for (JCoordinate y=0; y < (JCoordinate) lineHeight; y++)
		{
			const unsigned long pixelValue = tempImage->GetColor(x,y);
			JCoordinate xp,yp;
			if (quadrant == 2)
			{
				xp = +dy+y;
				yp = -dx-x;
			}
			else if (quadrant == 3)
			{
				xp = -dx-x;
				yp = -dy-y;
			}
			else	// quadrant == 4
			{
				xp = -dy-y;
				yp = +dx+x;
			}

			itsGC->SetDrawingColor(pixelValue);
			itsGC->DrawPoint(itsDrawable, x0+xp, y0+yp);
		}
	}

	// clean up

	jdelete tempImage;
	jdelete srcImage;

	itsGC->SetDrawingColor(GetFont().GetStyle().color);	// expected pen color
}

/******************************************************************************
 StyleString (private)

	Apply styles that the font id doesn't include.  If w>0, it is assumed
	to be the width of the string.

 ******************************************************************************/

void
JXWindowPainter::StyleString
	(
	const JString&		str,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	ascent,
	const JCoordinate	descent
	)
{
	const JFontStyle& fontStyle = GetFont().GetStyle();

	if (fontStyle.underlineCount > 0 || fontStyle.strike)
	{
		const JPoint origPenLoc     = GetPenLocation();
		const JColorID origPenColor = GetPenColor();
		const JSize origLW          = GetLineWidth();
		const bool wasDashed    = LinesAreDashed();

		SetPenColor(fontStyle.color);
		DrawDashedLines(false);

		const JSize strWidth = GetFont().GetStringWidth(GetFontManager(), str);

		if (fontStyle.underlineCount > 0)
		{
			const JSize ulWidth = GetFont().GetUnderlineThickness();
			SetLineWidth(ulWidth);

			JCoordinate yu = JLFloor(y + 1.5 * ulWidth);	// thick line is centered on path
			for (JIndex i=1; i<=fontStyle.underlineCount; i++)
			{
				Line(x,yu, x+strWidth,yu);
				yu += 2 * ulWidth;
			}
		}

		if (fontStyle.strike)
		{
			const JSize strikeWidth = GetFont().GetStrikeThickness();
			const JCoordinate ys    = y - ascent/2;		// thick line is centered on path
			SetLineWidth(strikeWidth);
			Line(x,ys, x+strWidth,ys);
		}

		SetPenLocation(origPenLoc);
		SetPenColor(origPenColor);
		SetLineWidth(origLW);
		DrawDashedLines(wasDashed);
	}
}

/******************************************************************************
 Point (virtual)

 ******************************************************************************/

void
JXWindowPainter::Point
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	const JPoint& o = GetOrigin();
	itsGC->SetDrawingColor(GetPenColor());
	itsGC->DrawPoint(itsDrawable, o.x+x, o.y+y);
}

/******************************************************************************
 Line (virtual)

 ******************************************************************************/

void
JXWindowPainter::Line
	(
	const JCoordinate x1,
	const JCoordinate y1,
	const JCoordinate x2,
	const JCoordinate y2
	)
{
	const JPoint& o = GetOrigin();
	itsGC->SetDrawingColor(GetPenColor());
	itsGC->SetLineWidth(GetLineWidth());
	itsGC->DrawDashedLines(LinesAreDashed());

	itsGC->DrawLine(itsDrawable, o.x+x1,o.y+y1, o.x+x2,o.y+y2);
	SetPenLocation(JPoint(x2,y2));
}

/******************************************************************************
 Rect (virtual)

 ******************************************************************************/

void
JXWindowPainter::Rect
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	const JPoint& o = GetOrigin();
	itsGC->SetDrawingColor(GetPenColor());
	itsGC->SetLineWidth(GetLineWidth());
	itsGC->DrawDashedLines(LinesAreDashed());
	const bool fill = IsFilling();
	if (!fill && w>0 && h>0)
	{
		itsGC->DrawRect(itsDrawable, o.x+x,o.y+y, w,h);
	}
	else if (fill && w>0 && h>0)
	{
		itsGC->FillRect(itsDrawable, o.x+x,o.y+y, w,h);
	}
}

/******************************************************************************
 Ellipse (virtual)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXWindowPainter::Ellipse
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	Arc(x,y, w,h, 0.0, 360.0);
}

/******************************************************************************
 Arc (virtual)

 ******************************************************************************/

void
JXWindowPainter::Arc
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
{
	if (w>0 && h>0)
	{
		const JPoint& o = GetOrigin();
		itsGC->SetDrawingColor(GetPenColor());
		itsGC->SetLineWidth(GetLineWidth());
		itsGC->DrawDashedLines(LinesAreDashed());
		itsGC->DrawArc(itsDrawable, o.x+x,o.y+y, w,h, startAngle, deltaAngle);

		if (IsFilling())
		{
			itsGC->FillArc(itsDrawable, o.x+x,o.y+y, w,h, startAngle, deltaAngle);
		}
	}
}

/******************************************************************************
 Polygon (virtual)

	To give consistent results, we always draw the outline of the polygon
	first, even when filling.

 ******************************************************************************/

void
JXWindowPainter::Polygon
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JPolygon&		poly
	)
{
	const JSize count = poly.GetElementCount();
	const JPoint& o   = GetOrigin();

	auto* xpt = jnew XPoint[ count+1 ];
	for (JSize i=0; i<count; i++)
	{
		const JPoint pt = poly.GetElement(i+1);
		xpt[i].x = o.x + left + pt.x;
		xpt[i].y = o.y + top  + pt.y;
	}
	const JPoint start = poly.GetElement(1);
	xpt[count].x = o.x + left + start.x;
	xpt[count].y = o.y + top  + start.y;

	itsGC->SetDrawingColor(GetPenColor());
	itsGC->SetLineWidth(GetLineWidth());
	itsGC->DrawDashedLines(LinesAreDashed());
	itsGC->DrawLines(itsDrawable, count+1, xpt);

	if (IsFilling())
	{
		itsGC->FillPolygon(itsDrawable, count+1, xpt);
	}

	jdelete [] xpt;
}

/******************************************************************************
 Image (virtual)

 ******************************************************************************/

void
JXWindowPainter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JRect&	destRect
	)
{
	const auto* ximage = dynamic_cast<const JXImage*>(&image);
	assert( ximage != nullptr );

	JRect destRectG = destRect;
	destRectG.Shift(GetOrigin());
	ximage->Draw(itsDrawable, itsGC, srcRect, destRectG);
}
