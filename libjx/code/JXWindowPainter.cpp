/******************************************************************************
 JXWindowPainter.cpp

	This class implements the functions required to draw to an X window.

	We don't provide functions to manipulate the default clip region because
	this is reserved for JXWindow, which constructs us.

	BASE CLASS = JPainter

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWindowPainter.h>
#include <JXDisplay.h>
#include <JXGC.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <JXImagePainter.h>
#include <jXUtil.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	defaultClipRegion can be NULL

 ******************************************************************************/

JXWindowPainter::JXWindowPainter
	(
	JXGC*			gc,
	Drawable		drawable,
	const JRect&	defaultClipRect,
	const Region	defaultClipRegion
	)
	:
	JPainter((gc->GetDisplay())->GetFontManager(), gc->GetColormap(),
			 defaultClipRect)
{
	assert( gc != NULL );

	itsDisplay        = gc->GetDisplay();
	itsDrawable       = drawable;
	itsGC             = gc;
	itsFontDrawable   = NULL;
	itsFontClipRegion = NULL;

	if (defaultClipRegion != NULL)
		{
		itsDefClipRegion = JXCopyRegion(defaultClipRegion);
		itsClipRegion    = JXCopyRegion(defaultClipRegion);
		}
	else
		{
		itsDefClipRegion = NULL;
		itsClipRegion    = NULL;
		}

	itsResetShouldClearClipRegionFlag = kJTrue;

	ResetClipRect();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindowPainter::~JXWindowPainter()
{
	if (itsClipRegion != NULL)
		{
		XDestroyRegion(itsClipRegion);
		}
	if (itsDefClipRegion != NULL)
		{
		XDestroyRegion(itsDefClipRegion);
		}
	if (itsFontDrawable != NULL)
		{
		XftDrawDestroy(itsFontDrawable);
		}
	if (itsFontClipRegion != NULL)
		{
		XDestroyRegion(itsFontClipRegion);
		}
}

/******************************************************************************
 GetXColormap

 ******************************************************************************/

JXColormap*
JXWindowPainter::GetXColormap()
	const
{
	return itsGC->GetColormap();
}

/******************************************************************************
 GetFontDrawable

 ******************************************************************************/

XftDraw*
JXWindowPainter::GetFontDrawable()
	const
{
	if (itsFontDrawable == NULL)
		{
		const_cast<JXWindowPainter*>(this)->itsFontDrawable =
			XftDrawCreate(*itsDisplay, itsDrawable,
						  itsDisplay->GetDefaultVisual(),
						  (itsDisplay->GetColormap())->GetXColormap());
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
		if (itsClipRegion != NULL)
			{
			XDestroyRegion(itsClipRegion);
			itsClipRegion = NULL;
			}

		if (itsDefClipRegion != NULL)
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
	itsResetShouldClearClipRegionFlag = kJFalse;
	JPainter::Reset(defClipRect);
	itsResetShouldClearClipRegionFlag = kJTrue;
}

/******************************************************************************
 SetClipRegion

	Must be in local coordinates.

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
	if (itsDefClipRegion != NULL)
		{
		XDestroyRegion(itsDefClipRegion);
		itsDefClipRegion = NULL;
		}

	if (region != NULL)
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
	if (itsClipRegion != NULL)
		{
		XDestroyRegion(itsClipRegion);
		}

	const JPoint& o = GetOrigin();
	XOffsetRegion(region, o.x, o.y);		// to global coords

	if (itsDefClipRegion != NULL)
		{
		itsClipRegion = XCreateRegion();
		XIntersectRegion(region, itsDefClipRegion, itsClipRegion);
		}
	else
		{
		itsClipRegion = JXCopyRegion(region);
		}

	XOffsetRegion(region, -o.x, -o.y);		// back to local coords
}

/******************************************************************************
 ResetClipRegion

 ******************************************************************************/

void
JXWindowPainter::ResetClipRegion()
{
	if (itsClipRegion != NULL)
		{
		XDestroyRegion(itsClipRegion);
		}

	if (itsDefClipRegion != NULL)
		{
		itsClipRegion = JXCopyRegion(itsDefClipRegion);
		}
	else
		{
		itsClipRegion = NULL;
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
	if (itsFontClipRegion != NULL)
		{
		XDestroyRegion(itsFontClipRegion);
		itsFontClipRegion = NULL;
		}

	const JRect r        = JPainter::SetClipRect(userRect);
	const JBoolean empty = r.IsEmpty();
	if (!empty)
		{
		JRect rG = r;
		rG.Shift(GetOrigin());		// to global

		if (itsClipRegion != NULL)
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

	if (itsFontDrawable != NULL)
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
JXWindowPainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	String(left, top, str, 0, width, hAlign, height, vAlign);
}

void
JXWindowPainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str,
	const JIndex		uIndex,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (JStringEmpty(str))
		{
		return;
		}

	const JFontID fontID        = GetFontID();
	const JSize fontSize        = GetFontSize();
	const JFontStyle& fontStyle = GetFontStyle();

	itsGC->SetFont(fontID);
	itsGC->SetDrawingColor(fontStyle.color);

	JCoordinate ascent, descent;
	GetLineHeight(&ascent, &descent);

	const JPoint& o = GetOrigin();
	JCoordinate x   = o.x + left;
	JCoordinate y   = o.y + top + ascent;
	AlignString(&x,&y, str, width, hAlign, height, vAlign);

	itsGC->DrawString(itsDrawable, GetFontDrawable(), x,y, str);
	if (uIndex > 0)
		{
		const JFontManager* fontMgr = GetFontManager();

		JCoordinate xu = x;
		if (uIndex > 1)
			{
			xu += fontMgr->GetStringWidth(fontID, fontSize, fontStyle,
										  str, uIndex-1);
			}

		const JCoordinate w =
			fontMgr->GetCharWidth(fontID, fontSize, fontStyle, str[uIndex-1]);

		const JCoordinate lineWidth = fontMgr->GetUnderlineThickness(fontSize);
		const JCoordinate yu        = y + JLFloor(1.5 * lineWidth);

		itsGC->SetLineWidth(lineWidth);
		itsGC->DrawDashedLines(kJFalse);
		itsGC->DrawLine(itsDrawable, xu, yu, xu+w-1, yu);
		}

	StyleString(str, x-o.x, y-o.y, ascent, descent, fontStyle.color);
}

void
JXWindowPainter::String
	(
	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (JStringEmpty(str))
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
		String(left, top, str, width, hAlign, height, vAlign);
		return;
		}

	// we have to do it pixel by pixel

	JXImage* srcImage = NULL;
	const JPoint& o   = GetOrigin();

	JCoordinate ascent, descent;
	const JSize lineHeight  = GetLineHeight(&ascent, &descent) + 1;
	const JSize stringWidth = GetStringWidth(str);

	const JCoordinate x0 = o.x + left;
	const JCoordinate y0 = o.y + top;

	JIndex quadrant;
	JCoordinate dx=0, dy=0;
	if (45.0 < angle && angle <= 135.0)
		{
		quadrant = 2;
		AlignString(&dx,&dy, str, height, hAlign, width, vAlign);
		srcImage = new JXImage(itsDisplay, itsDrawable,
							   JRect(y0-dx - stringWidth, x0+dy, y0-dx + 1, x0+dy + lineHeight + 1));
		}
	else if (135.0 < angle && angle <= 225.0)
		{
		quadrant = 3;
		AlignString(&dx,&dy, str, width, hAlign, height, vAlign);
		srcImage = new JXImage(itsDisplay, itsDrawable,
							   JRect(y0-dy - lineHeight, x0-dx - stringWidth, y0-dy + 1, x0-dx + 1));
		}
	else	// 225.0 < angle && angle <= 315.0
		{
		quadrant = 4;
		AlignString(&dx,&dy, str, height, hAlign, width, vAlign);
		srcImage = new JXImage(itsDisplay, itsDrawable,
							   JRect(y0+dx, x0-dy - lineHeight, y0+dx + stringWidth + 1, x0-dy + 1));
		}
	assert( srcImage != NULL );

	JXImage* tempImage = new JXImage(itsDisplay, stringWidth, lineHeight, 0, 0,
									 JXImage::kLocalStorage);
	assert( tempImage != NULL );

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
	p->SetFont(GetFontID(), GetFontSize(), GetFontStyle());
	p->String(0,0, str);
	delete p;
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

	delete tempImage;
	delete srcImage;

	itsGC->SetDrawingColor(GetFontStyle().color);	// expected pen color
}

/******************************************************************************
 StyleString (private)

	Apply styles that the font id doesn't include.

 ******************************************************************************/

void
JXWindowPainter::StyleString
	(
	const JCharacter* str,
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate ascent,
	const JCoordinate descent,
	const JColorIndex color
	)
{
	const JFontStyle& fontStyle = GetFontStyle();

	if (fontStyle.underlineCount > 0 || fontStyle.strike)
		{
		const JFontManager* fontManager = GetFontManager();

		const JPoint origPenLoc        = GetPenLocation();
		const JColorIndex origPenColor = GetPenColor();
		const JSize origLW             = GetLineWidth();
		const JBoolean wasDashed       = LinesAreDashed();

		SetPenColor(color);
		DrawDashedLines(kJFalse);

		const JSize fontSize = GetFontSize();
		const JSize strWidth = GetStringWidth(str);

		if (fontStyle.underlineCount > 0)
			{
			const JSize ulWidth = fontManager->GetUnderlineThickness(fontSize);
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
			const JSize strikeWidth = fontManager->GetStrikeThickness(fontSize);
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
	const JBoolean fill = IsFilling();
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

	XPoint* xpt = new XPoint[ count+1 ];
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

	delete [] xpt;
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
	const JXImage* ximage = dynamic_cast<const JXImage*>(&image);
	assert( ximage != NULL );

	JRect destRectG = destRect;
	destRectG.Shift(GetOrigin());
	ximage->Draw(itsDrawable, itsGC, srcRect, destRectG);
}
