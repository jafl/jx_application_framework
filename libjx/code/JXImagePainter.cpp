/******************************************************************************
 JXImagePainter.cpp

	This class implements the functions required to draw to a JXImage.
	We simply ensure that the JXImage is in Pixmap mode before calling
	JXWindowPainter.

	BASE CLASS = JXWindowPainter, virtual JBroadcaster

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXImagePainter.h"
#include "JXImage.h"
#include "JXGC.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

	defaultClipRegion can be nullptr

	We don't use the image's GC because our clipping could interfere
	with JXImage code.

 ******************************************************************************/

JXImagePainter::JXImagePainter
	(
	JXImage*		image,
	Drawable		drawable,
	const JRect&	defaultClipRect,
	const Region	defaultClipRegion
	)
	:
	JXWindowPainter(jnew JXGC(image->GetDisplay(), drawable),
					drawable, defaultClipRect, defaultClipRegion)
{
	assert( image != nullptr );

#ifndef NDEBUG
	{
	JXGC* gc = GetGC();
	assert( gc != nullptr );
	}
#endif

	itsImage = image;
	ClearWhenGoingAway(itsImage, &itsImage);
}

/******************************************************************************
 Destructor

	When we are finished, it is best to store the results as an XImage
	on the client side.

 ******************************************************************************/

JXImagePainter::~JXImagePainter()
{
	JXGC* gc = GetGC();
	jdelete gc;

	if (itsImage != nullptr)
		{
		itsImage->ConvertToDefaultState();
		}
}

/******************************************************************************
 String (virtual)

 ******************************************************************************/

void
JXImagePainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::String(left, top, str, width, hAlign, height, vAlign);
		}
}

void
JXImagePainter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JIndex		uIndex,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::String(left, top, str, uIndex, width, hAlign, height, vAlign);
		}
}

void
JXImagePainter::String
	(
	const JFloat		angle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::String(angle, left, top, str, width, hAlign, height, vAlign);
		}
}

/******************************************************************************
 Point (virtual)

 ******************************************************************************/

void
JXImagePainter::Point
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Point(x,y);
		}
}

/******************************************************************************
 Line (virtual)

 ******************************************************************************/

void
JXImagePainter::Line
	(
	const JCoordinate x1,
	const JCoordinate y1,
	const JCoordinate x2,
	const JCoordinate y2
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Line(x1,y1, x2,y2);
		}
}

/******************************************************************************
 Rect (virtual)

 ******************************************************************************/

void
JXImagePainter::Rect
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Rect(x,y,w,h);
		}
}

/******************************************************************************
 Ellipse (virtual)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXImagePainter::Ellipse
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Ellipse(x,y,w,h);
		}
}

/******************************************************************************
 Arc (virtual)

 ******************************************************************************/

void
JXImagePainter::Arc
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Arc(x,y,w,h, startAngle, deltaAngle);
		}
}

/******************************************************************************
 Polygon (virtual)

 ******************************************************************************/

void
JXImagePainter::Polygon
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JPolygon&		poly
	)
{
	if (PrepareXDraw())
		{
		JXWindowPainter::Polygon(left, top, poly);
		}
}

/******************************************************************************
 Image (virtual)

 ******************************************************************************/

void
JXImagePainter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JRect&	destRect
	)
{
	const JXImage* ximage = dynamic_cast<const JXImage*>(&image);
	assert( ximage != nullptr );

	if (PrepareXDraw())
		{
		JXWindowPainter::Image(image, srcRect, destRect);
		}
}

/******************************************************************************
 PrepareXDraw (private)

 ******************************************************************************/

JBoolean
JXImagePainter::PrepareXDraw()
{
	if (itsImage != nullptr)
		{
		itsImage->ConvertToPixmap();
		SetDrawable(itsImage->itsPixmap);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
