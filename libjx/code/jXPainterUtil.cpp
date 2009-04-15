/******************************************************************************
 jXPainterUtil.cpp

	Useful functions usable with JPainter objects but specific to X.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <jXPainterUtil.h>
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 JXDrawUpFrame

 ******************************************************************************/

void
JXDrawUpFrame
	(
	JPainter&			p,
	const JRect&		rect,
	const JCoordinate	width,
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();
	const JBoolean origFill     = p.IsFilling();

	p.SetLineWidth(1);

	// fill frame

	p.SetFilling(kJTrue);
	if (fill)
		{
		p.SetPenColor(fillColor);
		p.Rect(rect);
		}

	if (width < 3)
		{
		p.SetFilling(kJFalse);
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
	const JColormap* cmap = p.GetColormap();

	edge.AppendElement(topRight);
	edge.AppendElement(topRight1);
	edge.AppendElement(botRight1);
	edge.AppendElement(botLeft1);
	edge.AppendElement(botLeft);
	edge.AppendElement(botRight);
	p.SetPenColor(cmap->Get3DShadeColor());
	p.Polygon(edge);
	p.Point(botRight);

	edge.SetElement(1, topRight);
	edge.SetElement(2, topRight1);
	edge.SetElement(3, topLeft1);
	edge.SetElement(4, botLeft1);
	edge.SetElement(5, botLeft);
	edge.SetElement(6, topLeft);
	p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();
	const JBoolean origFill     = p.IsFilling();

	p.SetLineWidth(1);

	// fill frame

	p.SetFilling(kJTrue);
	if (fill)
		{
		p.SetPenColor(fillColor);
		p.Rect(rect);
		}

	if (width < 3)
		{
		p.SetFilling(kJFalse);
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
	const JColormap* cmap = p.GetColormap();

	edge.AppendElement(topRight);
	edge.AppendElement(topRight1);
	edge.AppendElement(topLeft1);
	edge.AppendElement(botLeft1);
	edge.AppendElement(botLeft);
	edge.AppendElement(topLeft);
	p.SetPenColor(cmap->Get3DShadeColor());
	p.Polygon(edge);

	edge.SetElement(1, topRight);
	edge.SetElement(2, topRight1);
	edge.SetElement(3, botRight1);
	edge.SetElement(4, botLeft1);
	edge.SetElement(5, botLeft);
	edge.SetElement(6, botRight);
	p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	fillColor
	)
{
	JRect rect = origRect;
	JXDrawDownFrame(p, rect, downWidth);
	rect.Shrink(downWidth, downWidth);
	if (betweenWidth > 0)
		{
		const JColorIndex origColor = p.GetPenColor();
		const JBoolean origFill     = p.IsFilling();
		p.SetPenColor((p.GetColormap())->GetDefaultBackColor());
		p.SetFilling(kJTrue);
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
	const JBoolean		fill,
	const JColorIndex	fillColor
	)
{
	JRect rect = origRect;
	JXDrawUpFrame(p, rect, upWidth);
	rect.Shrink(upWidth, upWidth);
	if (betweenWidth > 0)
		{
		const JColorIndex origColor = p.GetPenColor();
		const JBoolean origFill     = p.IsFilling();
		p.SetPenColor((p.GetColormap())->GetDefaultBackColor());
		p.SetFilling(kJTrue);
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
	const JColorIndex	borderColor,
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(11);

		edge.AppendElement(left);
		edge.AppendElement(left1);
		edge.AppendElement(top1);
		edge.AppendElement(right1);
		edge.AppendElement(bottom1);
		edge.AppendElement(left1);
		edge.AppendElement(left);
		edge.AppendElement(bottom);
		edge.AppendElement(right);
		edge.AppendElement(top);
		edge.AppendElement(left);
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw four lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(left, bottom);
		p.LineTo(right);

		p.SetPenColor(cmap->Get3DLightColor());
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

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(left);
		edge.AppendElement(left1);
		edge.AppendElement(bottom1);
		edge.AppendElement(right1);
		edge.AppendElement(right);
		edge.AppendElement(bottom);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(1, left);
		edge.SetElement(2, left1);
		edge.SetElement(3, top1);
		edge.SetElement(4, right1);
		edge.SetElement(5, right);
		edge.SetElement(6, top);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw four lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(left, top);
		p.LineTo(right);

		p.SetPenColor(cmap->Get3DLightColor());
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

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(left);
		edge.AppendElement(left1);
		edge.AppendElement(top1);
		edge.AppendElement(right1);
		edge.AppendElement(right);
		edge.AppendElement(top);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(1, left);
		edge.SetElement(2, left1);
		edge.SetElement(3, bottom1);
		edge.SetElement(4, right1);
		edge.SetElement(5, right);
		edge.SetElement(6, bottom);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JColorIndex	fillColor
	)
{
	const JBoolean origFill = p.IsFilling();

	const JCoordinate xc = rect.xcenter();
	const JCoordinate yc = rect.ycenter();

	JPolygon poly(4);
	poly.AppendElement(JPoint(xc, rect.top));
	poly.AppendElement(JPoint(rect.left, yc));
	poly.AppendElement(JPoint(xc, rect.bottom));
	poly.AppendElement(JPoint(rect.right, yc));

	p.SetPenColor(fillColor);
	p.SetFilling(kJTrue);
	p.Polygon(poly);
	p.Point(poly.GetElement(4));

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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(topRight, botRight);
		p.LineTo(left);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(topRight, left);
		}
	else
		{
		// use filled polygons

		const JPoint left1     = left     + JPoint( width-1, 0);
		const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(left1);
		edge.AppendElement(left);
		edge.AppendElement(botRight);
		edge.AppendElement(topRight);
		edge.AppendElement(topRight1);
		edge.AppendElement(botRight1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, topRight);
		edge.SetElement(4, topRight1);
		edge.RemoveElement(6);
		edge.RemoveElement(5);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(topRight, left);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(topRight, botRight);
		p.LineTo(left);
		}
	else
		{
		// use filled polygons

		const JPoint left1     = left     + JPoint( width-1, 0);
		const JPoint topRight1 = topRight + JPoint(-width+1,  width-1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(left1);
		edge.AppendElement(left);
		edge.AppendElement(topRight);
		edge.AppendElement(topRight1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, botRight);
		edge.SetElement(4, topRight);
		edge.AppendElement(topRight1);
		edge.AppendElement(botRight1);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JColorIndex	fillColor
	)
{
	const JBoolean origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendElement(JPoint(rect.left, rect.ycenter()));
	poly.AppendElement(rect.topRight()    + JPoint(-1, 0));
	poly.AppendElement(rect.bottomRight() + JPoint(-1,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(kJTrue);
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(botLeft, right);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(botLeft, topLeft);
		p.LineTo(right);
		}
	else
		{
		// use filled polygons

		const JPoint right1   = right   + JPoint(-width+1, 0);
		const JPoint topLeft1 = topLeft + JPoint( width-1,  width-1);
		const JPoint botLeft1 = botLeft + JPoint( width-1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(right1);
		edge.AppendElement(right);
		edge.AppendElement(botLeft);
		edge.AppendElement(botLeft1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, topLeft);
		edge.SetElement(4, botLeft);
		edge.AppendElement(botLeft1);
		edge.AppendElement(topLeft1);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
	const JSize origWidth       = p.GetLineWidth();

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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(botLeft, topLeft);
		p.LineTo(right);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(botLeft, right);
		}
	else
		{
		// use filled polygons

		const JPoint right1   = right   + JPoint(-width+1, 0);
		const JPoint topLeft1 = topLeft + JPoint( width-1,  width-1);
		const JPoint botLeft1 = botLeft + JPoint( width-1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(right1);
		edge.AppendElement(right);
		edge.AppendElement(topLeft);
		edge.AppendElement(botLeft);
		edge.AppendElement(botLeft1);
		edge.AppendElement(topLeft1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, botLeft);
		edge.SetElement(4, botLeft1);
		edge.RemoveElement(6);
		edge.RemoveElement(5);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JColorIndex	fillColor
	)
{
	const JBoolean origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendElement(JPoint(rect.right-1, rect.ycenter()));
	poly.AppendElement(rect.topLeft());
	poly.AppendElement(rect.bottomLeft() + JPoint(0,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(kJTrue);
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(botLeft, botRight);
		p.LineTo(top);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(botLeft, top);
		}
	else
		{
		// use filled polygons

		const JPoint top1      = top      + JPoint(0, width-1);
		const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(top1);
		edge.AppendElement(top);
		edge.AppendElement(botRight);
		edge.AppendElement(botLeft);
		edge.AppendElement(botLeft1);
		edge.AppendElement(botRight1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, botLeft);
		edge.SetElement(4, botLeft1);
		edge.RemoveElement(6);
		edge.RemoveElement(5);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(botLeft, top);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(botLeft, botRight);
		p.LineTo(top);
		}
	else
		{
		// use filled polygons

		const JPoint top1      = top      + JPoint(0, width-1);
		const JPoint botLeft1  = botLeft  + JPoint( width-1, -width+1);
		const JPoint botRight1 = botRight + JPoint(-width+1, -width+1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(top1);
		edge.AppendElement(top);
		edge.AppendElement(botLeft);
		edge.AppendElement(botLeft1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, botRight);
		edge.SetElement(4, botLeft);
		edge.AppendElement(botLeft1);
		edge.AppendElement(botRight1);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JColorIndex	fillColor
	)
{
	const JBoolean origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendElement(JPoint(rect.xcenter(), rect.top));
	poly.AppendElement(rect.bottomLeft()  + JPoint( 0,-1));
	poly.AppendElement(rect.bottomRight() + JPoint(-1,-1));

	p.SetPenColor(fillColor);
	p.SetFilling(kJTrue);
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(topRight, bottom);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(topRight, topLeft);
		p.LineTo(bottom);
		}
	else
		{
		// use filled polygons

		const JPoint bottom1   = bottom   + JPoint(0, -width+1);
		const JPoint topLeft1  = topLeft  + JPoint( width-1, width-1);
		const JPoint topRight1 = topRight + JPoint(-width+1, width-1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(bottom1);
		edge.AppendElement(bottom);
		edge.AppendElement(topRight);
		edge.AppendElement(topRight1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, topLeft);
		edge.SetElement(4, topRight);
		edge.AppendElement(topRight1);
		edge.AppendElement(topLeft1);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JBoolean		fill,
	const JColorIndex	origFillColor
	)
{
	if (width <= 0 || rect.IsEmpty())
		{
		return;
		}

	const JColorIndex fillColor =
		(origFillColor == kJXTransparentColor ?
		 (p.GetColormap())->GetBlackColor() : origFillColor);

	// save the original settings

	const JColorIndex origColor = p.GetPenColor();
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

	const JColormap* cmap = p.GetColormap();

	if (width == 1)
		{
		// optimize: draw three lines

		p.SetPenColor(cmap->Get3DShadeColor());
		p.Line(topRight, topLeft);
		p.LineTo(bottom);

		p.SetPenColor(cmap->Get3DLightColor());
		p.Line(topRight, bottom);
		}
	else
		{
		// use filled polygons

		const JPoint bottom1   = bottom   + JPoint(0, -width+1);
		const JPoint topLeft1  = topLeft  + JPoint( width-1, width-1);
		const JPoint topRight1 = topRight + JPoint(-width+1, width-1);

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);

		JPolygon edge(6);

		edge.AppendElement(bottom1);
		edge.AppendElement(bottom);
		edge.AppendElement(topLeft);
		edge.AppendElement(topRight);
		edge.AppendElement(topRight1);
		edge.AppendElement(topLeft1);
		p.SetPenColor(cmap->Get3DShadeColor());
		p.Polygon(edge);

		edge.SetElement(3, topRight);
		edge.SetElement(4, topRight1);
		edge.RemoveElement(6);
		edge.RemoveElement(5);
		p.SetPenColor(cmap->Get3DLightColor());
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
	const JColorIndex	fillColor
	)
{
	const JBoolean origFill = p.IsFilling();

	JPolygon poly(3);
	poly.AppendElement(JPoint(rect.xcenter(), rect.bottom));
	poly.AppendElement(rect.topLeft());
	poly.AppendElement(rect.topRight() + JPoint(-1,0));

	p.SetPenColor(fillColor);
	p.SetFilling(kJTrue);
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
		p.SetPenColor((p.GetColormap())->GetDefaultDNDBorderColor());
		p.JPainter::Rect(r);
		p.SetLineWidth(1);
		}
}
