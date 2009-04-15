/******************************************************************************
 JXWindowPainter.cpp

	This class implements the functions required to draw to an X window.

	We don't provide functions to manipulate the default clip region because
	this is reserved for JXWindow, which constructs us.

	BASE CLASS = JPainter

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWindowPainter.h>
#include <JXDisplay.h>
#include <JXGC.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <jXUtil.h>
#include <JFontManager.h>
#include <JString.h>
#include <string.h>
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

	itsDisplay   = gc->GetDisplay();
	itsDrawable  = drawable;
	itsGC        = gc;
	itsRotTextGC = NULL;

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

	delete itsRotTextGC;
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
			XDestroyRegion(clipRegion);
			}
		else
			{
			itsGC->SetClipRect(rG);
			}
		}
	else
		{
		itsGC->SetClipRect(r);
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

	itsGC->DrawString(itsDrawable, x,y, str);
	if (uIndex > 0)
		{
		const JFontManager* fontMgr = GetFontManager();

		JCoordinate xu = x;
		if (uIndex > 1)
			{
			xu += fontMgr->GetStringWidth(GetFontID(), GetFontSize(), GetFontStyle(),
										  str, uIndex-1);
			}

		const JCoordinate w =
			fontMgr->GetCharWidth(fontID, fontSize, fontStyle, str[uIndex-1]);

		const JCoordinate lineWidth = fontMgr->GetUnderlineThickness(fontSize);
		const JCoordinate yu        = y + JLFloor(1.5 * lineWidth);

		itsGC->SetDrawingColor(fontStyle.color);
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

	// Adjust the angle to lie between -45 and 315.

	JFloat angle = userAngle;
	while (angle <= -45.0)
		{
		angle += 360.0;
		}
	while (angle > 315.0)
		{
		angle -= 360.0;
		}

	// If the angle is zero, we can do it easily.

	JIndex quadrant;
	if (-45.0 < angle && angle <= 45.0)
		{
		String(left, top, str, width, hAlign, height, vAlign);
		return;
		}
	else if (45.0 < angle && angle <= 135.0)
		{
		quadrant = 2;
		}
	else if (135.0 < angle && angle <= 225.0)
		{
		quadrant = 3;
		}
	else	// 225.0 < angle && angle <= 315.0
		{
		quadrant = 4;
		}

	// We have to do it pixel by pixel.

	JCoordinate dx=0, dy=0;
	AlignString(&dx,&dy, str, width, hAlign, height, vAlign);

	JCoordinate ascent, descent;
	const JSize lineHeight  = GetLineHeight(&ascent, &descent) + 1;
	const JSize stringWidth = GetStringWidth(str);

	Pixmap tempPixmap =
		XCreatePixmap(*itsDisplay, itsDrawable, stringWidth, lineHeight,
					  DefaultDepth((Display*) *itsDisplay,
					  DefaultScreen((Display*) *itsDisplay)));
	assert( tempPixmap != 0 );

	JXColormap* colormap = itsDisplay->GetColormap();
	if (itsRotTextGC == NULL)
		{
		itsRotTextGC = new JXGC(itsDisplay, colormap, itsDrawable);
		assert( itsRotTextGC != NULL );
		}

	itsRotTextGC->SetFont(GetFontID());
	const JFontStyle& fontStyle = GetFontStyle();

	JXColormap* rotCMap = itsRotTextGC->GetColormap();
	itsRotTextGC->SetDrawingColor(rotCMap->GetWhiteColor());
	itsRotTextGC->FillRect(tempPixmap, 0,0, stringWidth,lineHeight);
	itsRotTextGC->SetDrawingColor(rotCMap->GetBlackColor());
	itsRotTextGC->DrawString(tempPixmap, 0, ascent, str);

	{
	const JPoint origOrigin     = GetOrigin();
	const Drawable origDrawable = itsDrawable;
	JXGC* origGC                = itsGC;

	SetOrigin(0,0);
	itsDrawable = tempPixmap;
	itsGC       = itsRotTextGC;

	StyleString(str, 0,ascent, ascent, descent, rotCMap->GetBlackColor());

	SetOrigin(origOrigin);
	itsDrawable = origDrawable;
	itsGC       = origGC;
	}

	XImage* tempImage =
		XGetImage(*itsDisplay, tempPixmap, 0,0, stringWidth, lineHeight, 0x01, ZPixmap);
	assert( tempImage != NULL );

	const JColorIndex foreColor = colormap->GetXPixel(colormap->GetBlackColor());
	const JColorIndex backColor = colormap->GetXPixel(colormap->GetWhiteColor());

	const JPoint& o = GetOrigin();
	itsGC->SetDrawingColor(fontStyle.color);
	JCoordinate xp,yp;
	for (JCoordinate x=0; x < (JCoordinate) stringWidth; x++)
		{
		for (JCoordinate y=0; y < (JCoordinate) lineHeight; y++)
			{
			const unsigned long pixelValue = XGetPixel(tempImage, x,y);
			if (pixelValue == foreColor)
				{
				if (quadrant == 2)
					{
					xp = left+dy+y;
					yp = top-dx-x;
					}
				else if (quadrant == 3)
					{
					xp = left-dx-x;
					yp = top-dy-y;
					}
				else
					{
					assert( quadrant == 4 );
					xp = left-dy-y;
					yp = top+dx+x;
					}

				itsGC->DrawPoint(itsDrawable, o.x+xp, o.y+yp);
				}
			}
		}

	XDestroyImage(tempImage);
	XFreePixmap(*itsDisplay, tempPixmap);
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
	const JXImage* ximage = dynamic_cast(const JXImage*, &image);
	assert( ximage != NULL );

	JRect destRectG = destRect;
	destRectG.Shift(GetOrigin());
	ximage->Draw(itsDrawable, itsGC, srcRect, destRectG);
}
