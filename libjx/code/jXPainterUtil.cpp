/******************************************************************************
 jXPainterUtil.cpp

	Useful functions usable with JPainter objects but specific to X.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jXPainterUtil.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 JXDrawUpFrame

 ******************************************************************************/

void
JXDrawUpFrame
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool			fill,
	const JColorID		origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth    = p.GetLineWidth();
	const bool origFill      = p.IsFilling();

	p.SetLineWidth(1);

	// fill frame

	p.SetFilling(true);
	if (fill)
	{
		p.SetPenColor(fillColor);
		p.Rect(rect);
	}

	if (width < 3)
	{
		p.SetFilling(false);
	}

	// calculate the vertices

	const JPoint topLeft  = rect.topLeft();
	const JPoint botLeft  = rect.bottomLeft()  + JPoint( 0,-1);
	const JPoint topRight = rect.topRight()    + JPoint(-1, 0);
	const JPoint botRight = rect.bottomRight() + JPoint(-1,-1);

	// draw the four sides

	const JPoint topLeft1  = topLeft  + JPoint( width-1,  width-1);
	const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
	const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
	const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

	JPolygon edge(6);

	edge.AppendItem(topRight);
	edge.AppendItem(topRight1);
	edge.AppendItem(botRight1);
	edge.AppendItem(botLeft1);
	edge.AppendItem(botLeft);
	edge.AppendItem(botRight);
	p.SetPenColor(JColorManager::Get3DShadeColor());
	p.Polygon(edge);
	p.Point(botRight);

	edge.SetItem(1, topRight);
	edge.SetItem(2, topRight1);
	edge.SetItem(3, topLeft1);
	edge.SetItem(4, botLeft1);
	edge.SetItem(5, botLeft);
	edge.SetItem(6, topLeft);
	p.SetPenColor(JColorManager::Get3DLightColor());
	p.Polygon(edge);

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawDownFrame

 ******************************************************************************/

void
JXDrawDownFrame
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool			fill,
	const JColorID		origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth    = p.GetLineWidth();
	const bool origFill      = p.IsFilling();

	p.SetLineWidth(1);

	// fill frame

	p.SetFilling(true);
	if (fill)
	{
		p.SetPenColor(fillColor);
		p.Rect(rect);
	}

	if (width < 3)
	{
		p.SetFilling(false);
	}

	// calculate the vertices

	const JPoint topLeft   = rect.topLeft();
	const JPoint botLeft   = rect.bottomLeft()  + JPoint( 0,-1);
	const JPoint topRight  = rect.topRight()    + JPoint(-1, 0);
	const JPoint botRight  = rect.bottomRight() + JPoint(-1,-1);

	// draw the four sides

	const JPoint topLeft1  = topLeft  + JPoint( width-1,  width-1);
	const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
	const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
	const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

	JPolygon edge(6);

	edge.AppendItem(topRight);
	edge.AppendItem(topRight1);
	edge.AppendItem(topLeft1);
	edge.AppendItem(botLeft1);
	edge.AppendItem(botLeft);
	edge.AppendItem(topLeft);
	p.SetPenColor(JColorManager::Get3DShadeColor());
	p.Polygon(edge);

	edge.SetItem(1, topRight);
	edge.SetItem(2, topRight1);
	edge.SetItem(3, botRight1);
	edge.SetItem(4, botLeft1);
	edge.SetItem(5, botLeft);
	edge.SetItem(6, botRight);
	p.SetPenColor(JColorManager::Get3DLightColor());
	p.Polygon(edge);
	p.Point(botRight);

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawEngravedFrame

	Draw a frame that appears to be engraved (chiseled) into the background.
	The depth down and the height back up can be different.

 ******************************************************************************/

void
JXDrawEngravedFrame
	(
	JPainter&			p,
	const JRect&		origRect,
	const JCoordinate	downWidth,
	const JCoordinate	betweenWidth,
	const JCoordinate	upWidth,
	const bool			fill,
	const JColorID		fillColor
	)
{
	JRect rect = origRect;
	JXDrawDownFrame(p, rect, downWidth);
	rect.Shrink(downWidth, downWidth);
	if (betweenWidth > 0)
	{
		const JColorID origColor = p.GetPenColor();
		const bool origFill     = p.IsFilling();
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.Rect(rect);
		p.SetPenColor(origColor);
		p.SetFilling(origFill);

		rect.Shrink(betweenWidth, betweenWidth);
	}
	JXDrawUpFrame(p, rect, upWidth, fill, fillColor);
}

/******************************************************************************
 JXDrawEmbossedFrame

	Draw a frame that appears to be embossed (raised) above the background.
	The height up and the depth back down can be different.

 ******************************************************************************/

void
JXDrawEmbossedFrame
	(
	JPainter&			p,
	const JRect&		origRect,
	const JCoordinate	upWidth,
	const JCoordinate	betweenWidth,
	const JCoordinate	downWidth,
	const bool			fill,
	const JColorID		fillColor
	)
{
	JRect rect = origRect;
	JXDrawUpFrame(p, rect, upWidth);
	rect.Shrink(upWidth, upWidth);
	if (betweenWidth > 0)
	{
		const JColorID origColor = p.GetPenColor();
		const bool origFill     = p.IsFilling();
		p.SetPenColor(JColorManager::GetDefaultBackColor());
		p.SetFilling(true);
		p.Rect(rect);
		p.SetPenColor(origColor);
		p.SetFilling(origFill);

		rect.Shrink(betweenWidth, betweenWidth);
	}
	JXDrawDownFrame(p, rect, downWidth, fill, fillColor);
}

/******************************************************************************
 JXDrawFlatDiamond

 ******************************************************************************/

void
JXDrawFlatDiamond
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const JColorID	borderColor,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill diamond

	if (fill)
	{
		JXFillDiamond(p, rect, fillColor);
	}

	// calculate the vertices

	const JCoordinate xc = rect.xcenter();
	const JCoordinate yc = rect.ycenter();

	const JPoint top   (xc, rect.top);
	const JPoint left  (rect.left, yc);
	const JPoint bottom(xc, rect.bottom);
	const JPoint right (rect.right, yc);

	// draw the four sides

	p.SetPenColor(borderColor);

	if (width == 1)
	{
		// optimize: draw four lines

		p.Line(left, top);
		p.LineTo(right);
		p.LineTo(bottom);
		p.LineTo(left);
	}
	else
	{
		// use filled polygon

		const JPoint top1    = top    + JPoint(0, width-1);
		const JPoint left1   = left   + JPoint(width-1, 0);
		const JPoint bottom1 = bottom - JPoint(0, width-1);
		const JPoint right1  = right  - JPoint(width-1, 0);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(11);

		edge.AppendItem(left);
		edge.AppendItem(left1);
		edge.AppendItem(top1);
		edge.AppendItem(right1);
		edge.AppendItem(bottom1);
		edge.AppendItem(left1);
		edge.AppendItem(left);
		edge.AppendItem(bottom);
		edge.AppendItem(right);
		edge.AppendItem(top);
		edge.AppendItem(left);
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawUpDiamond

 ******************************************************************************/

void
JXDrawUpDiamond
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill diamond

	if (fill)
	{
		JXFillDiamond(p, rect, fillColor);
	}

	// calculate the vertices

	const JCoordinate xc = rect.xcenter();
	const JCoordinate yc = rect.ycenter();

	const JPoint top   (xc, rect.top);
	const JPoint left  (rect.left, yc);
	const JPoint bottom(xc, rect.bottom);
	const JPoint right (rect.right, yc);

	// draw the four sides

	if (width == 1)
	{
		// optimize: draw four lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(left, bottom);
		p.LineTo(right);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(left, top);
		p.LineTo(right + JPoint(1,1));
	}
	else
	{
		// use filled polygons

		const JPoint top1    = top    + JPoint(0, width-1);
		const JPoint left1   = left   + JPoint(width-1, 0);
		const JPoint bottom1 = bottom + JPoint(0, -width+1);
		const JPoint right1  = right  + JPoint(-width+1, 0);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(left);
		edge.AppendItem(left1);
		edge.AppendItem(bottom1);
		edge.AppendItem(right1);
		edge.AppendItem(right);
		edge.AppendItem(bottom);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(1, left);
		edge.SetItem(2, left1);
		edge.SetItem(3, top1);
		edge.SetItem(4, right1);
		edge.SetItem(5, right);
		edge.SetItem(6, top);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawDownDiamond

 ******************************************************************************/

void
JXDrawDownDiamond
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill diamond

	if (fill)
	{
		JXFillDiamond(p, rect, fillColor);
	}

	// calculate the vertices

	const JCoordinate xc = rect.xcenter();
	const JCoordinate yc = rect.ycenter();

	const JPoint top   (xc, rect.top);
	const JPoint left  (rect.left, yc);
	const JPoint bottom(xc, rect.bottom);
	const JPoint right (rect.right, yc);

	// draw the four sides

	if (width == 1)
	{
		// optimize: draw four lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(left, top);
		p.LineTo(right);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(left, bottom);
		p.LineTo(right + JPoint(1,-1));
	}
	else
	{
		// use filled polygons

		const JPoint top1    = top    + JPoint(0, width-1);
		const JPoint left1   = left   + JPoint(width-1, 0);
		const JPoint bottom1 = bottom + JPoint(0, -width+1);
		const JPoint right1  = right  + JPoint(-width+1, 0);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(left);
		edge.AppendItem(left1);
		edge.AppendItem(top1);
		edge.AppendItem(right1);
		edge.AppendItem(right);
		edge.AppendItem(top);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(1, left);
		edge.SetItem(2, left1);
		edge.SetItem(3, bottom1);
		edge.SetItem(4, right1);
		edge.SetItem(5, right);
		edge.SetItem(6, bottom);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXFillDiamond

 ******************************************************************************/

void
JXFillDiamond
	(
	JPainter&			p,
	const JRect&		rect,
	const JColorID	fillColor
	)
{
	const bool origFill = p.IsFilling();

	const JCoordinate xc = rect.xcenter();
	const JCoordinate yc = rect.ycenter();

	JPolygon poly(4);
	poly.AppendItem(JPoint(xc, rect.top));
	poly.AppendItem(JPoint(rect.left, yc));
	poly.AppendItem(JPoint(xc, rect.bottom));
	poly.AppendItem(JPoint(rect.right, yc));

	p.SetPenColor(fillColor);
	p.SetFilling(true);
	p.Polygon(poly);
	p.Point(poly.GetItem(4));

	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawUpArrowLeft

 ******************************************************************************/

void
JXDrawUpArrowLeft
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool			fill,
	const JColorID		origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth    = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowLeft(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint left(rect.left, rect.ycenter());
	const JPoint topRight = rect.topRight()    + JPoint(-1, 0);
	const JPoint botRight = rect.bottomRight() + JPoint(-1,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(topRight, botRight);
		p.LineTo(left);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(topRight, left);
	}
	else
	{
		// use filled polygons

		const JPoint left1     = left     + JPoint( width-1, 0);
		const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(left1);
		edge.AppendItem(left);
		edge.AppendItem(botRight);
		edge.AppendItem(topRight);
		edge.AppendItem(topRight1);
		edge.AppendItem(botRight1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, topRight);
		edge.SetItem(4, topRight1);
		edge.RemoveItem(6);
		edge.RemoveItem(5);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawDownArrowLeft

 ******************************************************************************/

void
JXDrawDownArrowLeft
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool			fill,
	const JColorID		origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowLeft(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint left(rect.left, rect.ycenter());
	const JPoint topRight = rect.topRight()    + JPoint(-1, 0);
	const JPoint botRight = rect.bottomRight() + JPoint(-1,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(topRight, left);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(topRight, botRight);
		p.LineTo(left);
	}
	else
	{
		// use filled polygons

		const JPoint left1     = left     + JPoint( width-1, 0);
		const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(left1);
		edge.AppendItem(left);
		edge.AppendItem(topRight);
		edge.AppendItem(topRight1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, botRight);
		edge.SetItem(4, topRight);
		edge.AppendItem(topRight1);
		edge.AppendItem(botRight1);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXFillArrowLeft

 ******************************************************************************/

void
JXFillArrowLeft
	(
	JPainter&			p,
	const JRect&		rect,
	const JColorID	fillColor
	)
{
	const bool origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendItem(JPoint(rect.left, rect.ycenter()));
	poly.AppendItem(rect.topRight()    + JPoint(-1, 0));
	poly.AppendItem(rect.bottomRight() + JPoint(-1,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(true);
	p.Polygon(poly);

	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawUpArrowRight

 ******************************************************************************/

void
JXDrawUpArrowRight
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth    = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowRight(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint right(rect.right-1, rect.ycenter());
	const JPoint topLeft = rect.topLeft();
	const JPoint botLeft = rect.bottomLeft() + JPoint(0,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(botLeft, right);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(botLeft, topLeft);
		p.LineTo(right);
	}
	else
	{
		// use filled polygons

		const JPoint right1   = right   + JPoint(-width+1, 0);
		const JPoint topLeft1 = topLeft + JPoint( width-1,  width-1);
		const JPoint botLeft1 = botLeft + JPoint( width-1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(right1);
		edge.AppendItem(right);
		edge.AppendItem(botLeft);
		edge.AppendItem(botLeft1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, topLeft);
		edge.SetItem(4, botLeft);
		edge.AppendItem(botLeft1);
		edge.AppendItem(topLeft1);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawDownArrowRight

 ******************************************************************************/

void
JXDrawDownArrowRight
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool			fill,
	const JColorID		origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth    = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowRight(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint right(rect.right-1, rect.ycenter());
	const JPoint topLeft = rect.topLeft();
	const JPoint botLeft = rect.bottomLeft() + JPoint(0,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(botLeft, topLeft);
		p.LineTo(right);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(botLeft, right);
	}
	else
	{
		// use filled polygons

		const JPoint right1   = right   + JPoint(-width+1, 0);
		const JPoint topLeft1 = topLeft + JPoint( width-1,  width-1);
		const JPoint botLeft1 = botLeft + JPoint( width-1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(right1);
		edge.AppendItem(right);
		edge.AppendItem(topLeft);
		edge.AppendItem(botLeft);
		edge.AppendItem(botLeft1);
		edge.AppendItem(topLeft1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, botLeft);
		edge.SetItem(4, botLeft1);
		edge.RemoveItem(6);
		edge.RemoveItem(5);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXFillArrowRight

 ******************************************************************************/

void
JXFillArrowRight
	(
	JPainter&			p,
	const JRect&		rect,
	const JColorID	fillColor
	)
{
	const bool origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendItem(JPoint(rect.right-1, rect.ycenter()));
	poly.AppendItem(rect.topLeft());
	poly.AppendItem(rect.bottomLeft() + JPoint(0,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(true);
	p.Polygon(poly);

	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawUpArrowUp

 ******************************************************************************/

void
JXDrawUpArrowUp
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowUp(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint top(rect.xcenter(), rect.top);
	const JPoint botLeft  = rect.bottomLeft()  + JPoint( 0,-1);
	const JPoint botRight = rect.bottomRight() + JPoint(-1,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(botLeft, botRight);
		p.LineTo(top);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(botLeft, top);
	}
	else
	{
		// use filled polygons

		const JPoint top1      = top      + JPoint(0, width-1);
		const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(top1);
		edge.AppendItem(top);
		edge.AppendItem(botRight);
		edge.AppendItem(botLeft);
		edge.AppendItem(botLeft1);
		edge.AppendItem(botRight1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, botLeft);
		edge.SetItem(4, botLeft1);
		edge.RemoveItem(6);
		edge.RemoveItem(5);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawDownArrowUp

 ******************************************************************************/

void
JXDrawDownArrowUp
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowUp(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint top(rect.xcenter(), rect.top);
	const JPoint botLeft  = rect.bottomLeft()  + JPoint( 0,-1);
	const JPoint botRight = rect.bottomRight() + JPoint(-1,-1);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(botLeft, top);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(botLeft, botRight);
		p.LineTo(top);
	}
	else
	{
		// use filled polygons

		const JPoint top1      = top      + JPoint(0, width-1);
		const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(top1);
		edge.AppendItem(top);
		edge.AppendItem(botLeft);
		edge.AppendItem(botLeft1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, botRight);
		edge.SetItem(4, botLeft);
		edge.AppendItem(botLeft1);
		edge.AppendItem(botRight1);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXFillArrowUp

 ******************************************************************************/

void
JXFillArrowUp
	(
	JPainter&			p,
	const JRect&		rect,
	const JColorID	fillColor
	)
{
	const bool origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendItem(JPoint(rect.xcenter(), rect.top));
	poly.AppendItem(rect.bottomLeft()  + JPoint( 0,-1));
	poly.AppendItem(rect.bottomRight() + JPoint(-1,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(true);
	p.Polygon(poly);

	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawUpArrowDown

 ******************************************************************************/

void
JXDrawUpArrowDown
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowDown(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint bottom(rect.xcenter(), rect.bottom);
	const JPoint topLeft  = rect.topLeft();
	const JPoint topRight = rect.topRight() + JPoint(-1,0);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(topRight, bottom);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(topRight, topLeft);
		p.LineTo(bottom);
	}
	else
	{
		// use filled polygons

		const JPoint bottom1   = bottom   + JPoint(0, -width+1);
		const JPoint topLeft1  = topLeft  + JPoint( width-1, width-1);
		const JPoint topRight1 = topRight + JPoint(-width+1, width-1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(bottom1);
		edge.AppendItem(bottom);
		edge.AppendItem(topRight);
		edge.AppendItem(topRight1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, topLeft);
		edge.SetItem(4, topRight);
		edge.AppendItem(topRight1);
		edge.AppendItem(topLeft1);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXDrawDownArrowDown

 ******************************************************************************/

void
JXDrawDownArrowDown
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const bool		fill,
	const JColorID	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
	{
		return;
	}

	const JColorID fillColor =
		(origFillColor == kJXTransparentColor ?
		 JColorManager::GetBlackColor() : origFillColor);

	// save the original settings

	const JColorID origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

	p.SetLineWidth(1);

	// fill frame

	if (fill)
	{
		JXFillArrowDown(p, rect, fillColor);
	}

	// calculate the vertices

	const JPoint bottom(rect.xcenter(), rect.bottom);
	const JPoint topLeft  = rect.topLeft();
	const JPoint topRight = rect.topRight() + JPoint(-1,0);

	// draw the three sides

	if (width == 1)
	{
		// optimize: draw three lines

		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Line(topRight, topLeft);
		p.LineTo(bottom);

		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Line(topRight, bottom);
	}
	else
	{
		// use filled polygons

		const JPoint bottom1   = bottom   + JPoint(0, -width+1);
		const JPoint topLeft1  = topLeft  + JPoint( width-1, width-1);
		const JPoint topRight1 = topRight + JPoint(-width+1, width-1);

		const bool origFill = p.IsFilling();
		p.SetFilling(true);

		JPolygon edge(6);

		edge.AppendItem(bottom1);
		edge.AppendItem(bottom);
		edge.AppendItem(topLeft);
		edge.AppendItem(topRight);
		edge.AppendItem(topRight1);
		edge.AppendItem(topLeft1);
		p.SetPenColor(JColorManager::Get3DShadeColor());
		p.Polygon(edge);

		edge.SetItem(3, topRight);
		edge.SetItem(4, topRight1);
		edge.RemoveItem(6);
		edge.RemoveItem(5);
		p.SetPenColor(JColorManager::Get3DLightColor());
		p.Polygon(edge);

		p.SetFilling(origFill);
	}

	// clean up

	p.SetPenColor(origColor);
	p.SetLineWidth(origWidth);
}

/******************************************************************************
 JXFillArrowDown

 ******************************************************************************/

void
JXFillArrowDown
	(
	JPainter&			p,
	const JRect&		rect,
	const JColorID	fillColor
	)
{
	const bool origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendItem(JPoint(rect.xcenter(), rect.bottom));
	poly.AppendItem(rect.topLeft());
	poly.AppendItem(rect.topRight() + JPoint(-1,0));

	p.SetPenColor(fillColor);
	p.SetFilling(true);
	p.Polygon(poly);

	p.SetFilling(origFill);
}

/******************************************************************************
 JXDrawDNDBorder

	Use this to draw a blue border around a widget when a drop will be
	accepted.

 ******************************************************************************/

void
JXDrawDNDBorder
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width
	)
{
	if (width > 0)
	{
		JRect r   = rect;		// adjust for X centering thickness around line
		r.top    += width/2;
		r.left   += width/2;
		r.bottom -= (width-1)/2;
		r.right  -= (width-1)/2;

		p.SetLineWidth(width);
		p.SetPenColor(JColorManager::GetDefaultDNDBorderColor());
		p.Rect(r);
		p.SetLineWidth(1);
	}
}
