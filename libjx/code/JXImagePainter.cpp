/******************************************************************************
 JXImagePainter.cpp

	This class implements the functions required to draw to a JXImage.
	We simply insure that the JXImage is in Pixmap mode before calling
	JXWindowPainter.

	BASE CLASS = JXWindowPainter, virtual JBroadcaster

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImagePainter.h>
#include <JXImage.h>
#include <JXGC.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	defaultClipRegion can be NULL

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
	JXWindowPainter(new JXGC(image->GetDisplay(), image->GetXColormap(), drawable),
					drawable, defaultClipRect, defaultClipRegion)
{
	assert( image != NULL );

#ifndef NDEBUG
	{
	JXGC* gc = GetGC();
	assert( gc != NULL );
	}
#endif

	itsImage = image;
	ListenTo(itsImage);
}

/******************************************************************************
 Destructor

	When we are finished, it is best to store the results as an XImage
	on the client side.

 ******************************************************************************/

JXImagePainter::~JXImagePainter()
{
	JXGC* gc = GetGC();
	delete gc;

	if (itsImage != NULL)
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
	const JCharacter*	str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (PrepareText())
		{
		JXWindowPainter::String(left, top, str, width, hAlign, height, vAlign);
		}
}

void
JXImagePainter::String
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
	if (PrepareText())
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
	const JCharacter*	str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	if (PrepareText())
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
	if (PrepareDraw())
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
	if (PrepareDraw())
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
	if (PrepareDraw())
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
	if (PrepareDraw())
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
	if (PrepareDraw())
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
	if (PrepareDraw())
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
	const JXImage* ximage = dynamic_cast(const JXImage*, &image);
	assert( ximage != NULL );

	if (PrepareImage(*ximage))
		{
		JXWindowPainter::Image(image, srcRect, destRect);
		}
}

/******************************************************************************
 PrepareText (private)

 ******************************************************************************/

JBoolean
JXImagePainter::PrepareText()
{
	if (PrepareXDraw())
		{
		const JFontStyle& style = GetFontStyle();
		itsImage->RegisterColor(style.color);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareDraw (private)

 ******************************************************************************/

JBoolean
JXImagePainter::PrepareDraw()
{
	if (PrepareXDraw())
		{
		itsImage->RegisterColor(GetPenColor());
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareImage (private)

 ******************************************************************************/

JBoolean
JXImagePainter::PrepareImage
	(
	const JXImage& image
	)
{
	if (PrepareXDraw())
		{
		itsImage->RegisterColors(*(image.itsColorList));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareXDraw (private)

 ******************************************************************************/

JBoolean
JXImagePainter::PrepareXDraw()
{
	if (itsImage != NULL)
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

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JXImagePainter::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsImage)
		{
		itsImage = NULL;
		}
}
