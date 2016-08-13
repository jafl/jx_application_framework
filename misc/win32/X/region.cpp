/******************************************************************************
 region.cpp

	Region functions.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XStdInc.h"
#include "private.h"
#include <assert.h>

/******************************************************************************
 XCreateRegion

 ******************************************************************************/

Region
XCreateRegion()
{
	HRGN region = CreateRectRgn(0, 0, 0, 0);
	return (Region) region;
}

/******************************************************************************
 XCreateRegion

	TODO: implement fillRule

 ******************************************************************************/

Region
XPolygonRegion
	(
    XPoint*	points,
    int		n,
    int		fillRule
	)
{
	POINT* pt = new POINT[ n ];
	assert( pt != NULL );

	for (int i=0; i<n; i++)
		{
		pt[i].x = points[i].x;
		pt[i].y = points[i].y;
		}

	HRGN region = CreatePolygonRgn(pt, n, ALTERNATE);

	delete [] pt;
	return (Region) region;
}

/******************************************************************************
 XDestroyRegion

 ******************************************************************************/

int
XDestroyRegion
	(
	Region region
	)
{
	if (region != NULL)
		{
		DeleteObject((HRGN) region);
		}
	return TRUE;
}

/******************************************************************************
 XEmptyRegion

 ******************************************************************************/

int
XEmptyRegion
	(
	Region region
	)
{
	SetRectRgn((HRGN) region, 0, 0, 0, 0);
	return TRUE;
}

/******************************************************************************
 XOffsetRegion

 ******************************************************************************/

int
XOffsetRegion
	(
	Region	region,
	int		dx,
	int		dy
	)
{
	const int result = OffsetRgn((HRGN) region, dx, dy);
	assert( result != ERROR );
	return TRUE;
}

/******************************************************************************
 XIntersectRegion

 ******************************************************************************/

int
XIntersectRegion
	(
    Region src1Region,
    Region src2Region,
    Region destRegion
	)
{
	const int result = CombineRgn((HRGN) destRegion, (HRGN) src1Region, (HRGN) src2Region, RGN_AND);
	assert( result != ERROR );
	return TRUE;
}

/******************************************************************************
 XUnionRegion

 ******************************************************************************/

int
XUnionRegion
	(
    Region src1Region,
    Region src2Region,
    Region destRegion
	)
{
	const int result = CombineRgn((HRGN) destRegion, (HRGN) src1Region, (HRGN) src2Region, RGN_OR);
	assert( result != ERROR );
	return TRUE;
}

/******************************************************************************
 XUnionRectWithRegion

 ******************************************************************************/

int
XUnionRectWithRegion
	(
    XRectangle*	rectangle,
    Region		srcRegion,
    Region		destRegion
	)
{
	HRGN rectRegion = CreateRectRgn(rectangle->x, rectangle->y,
									rectangle->x + rectangle->width,
									rectangle->y + rectangle->height);
	XUnionRegion((Region) rectRegion, srcRegion, destRegion);
	DeleteObject(rectRegion);
	return TRUE;
}

/******************************************************************************
 XSubtractRegion

	TODO:  is RGN_DIFF the correct mode?  Are the args in the right order?

 ******************************************************************************/

int
XSubtractRegion
	(
    Region src1Region,
    Region src2Region,
    Region destRegion
	)
{
	const int result = CombineRgn((HRGN) destRegion, (HRGN) src1Region, (HRGN) src2Region, RGN_DIFF);
	assert( result != ERROR );
	return TRUE;
}

/******************************************************************************
 XRectInRegion

	TODO:  is this the correct Windows function to call?

 ******************************************************************************/

int
XRectInRegion
	(
    Region	region,
    int		x,
	int		y,
	int		width,
	int		height
	)
{
	RECT r = { x, y, x+width, y+height };
	return RectInRegion((HRGN) region, &r);
}
