/******************************************************************************
 TestPainter.cpp

	BASE CLASS = JPainter

	Written by John Lindal.

 ******************************************************************************/

#include "TestPainter.h"
#include "TestFontManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPainter::TestPainter()
	:
	JPainter(jnew TestFontManager, JRect())
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPainter::~TestPainter()
{
}

/******************************************************************************
 Reset (virtual)

 ******************************************************************************/

void
TestPainter::Reset()
{
	JPainter::Reset();
}

/******************************************************************************
 SetClipRect (virtual)

 ******************************************************************************/

JRect
TestPainter::SetClipRect
	(
	const JRect& userRect
	)
{
	return JRect();
}

/******************************************************************************
 SetDashList (virtual)

 ******************************************************************************/

void
TestPainter::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	JPainter::SetDashList(dashList, dashOffset);
}

/******************************************************************************
 String (virtual)

 ******************************************************************************/

void
TestPainter::String
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
	std::cout << str << std::endl;
}

void
TestPainter::String
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
	std::cout << str << std::endl;
}

void
TestPainter::String
	(
	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	std::cout << str << std::endl;
}

/******************************************************************************
 Point (virtual)

 ******************************************************************************/

void
TestPainter::Point
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
}

/******************************************************************************
 Line (virtual)

 ******************************************************************************/

void
TestPainter::Line
	(
	const JCoordinate x1,
	const JCoordinate y1,
	const JCoordinate x2,
	const JCoordinate y2
	)
{
}

/******************************************************************************
 Rect (virtual)

 ******************************************************************************/

void
TestPainter::Rect
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
}

/******************************************************************************
 Ellipse (virtual)

	Not inline because it is virtual.

 ******************************************************************************/

void
TestPainter::Ellipse
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
}

/******************************************************************************
 Arc (virtual)

 ******************************************************************************/

void
TestPainter::Arc
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
{
}

/******************************************************************************
 Polygon (virtual)

	To give consistent results, we always draw the outline of the polygon
	first, even when filling.

 ******************************************************************************/

void
TestPainter::Polygon
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JPolygon&		poly
	)
{
}

/******************************************************************************
 Image (virtual)

 ******************************************************************************/

void
TestPainter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JRect&	destRect
	)
{
}
