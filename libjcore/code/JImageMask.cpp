/******************************************************************************
 JImageMask.cpp

	Pure virtual base class to represent a mask for an offscreen picture.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JImageMask.h>
#include <JImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JImageMask::JImageMask()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JImageMask::JImageMask
	(
	const JImageMask& source
	)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JImageMask::~JImageMask()
{
}

/******************************************************************************
 CalcMask (protected)

	*** We must be the same size as the image.

	Note that by comparing system dependent colors instead of JColorIndices,
	we run in O(W H) instead of O(W H C), where C is the number of colors
	in the colormap.  The disadvantage is that system colors may be approximate
	so two colors close together will have the same system value, but different
	JColorIndices.  For masking, this should not be a problem, however, because
	one usually masks out a color that is very different from the other
	colors in the image.

 ******************************************************************************/

void
JImageMask::CalcMask
	(
	const JImage&		image,
	const JColorIndex	color
	)
{
	const unsigned long sysColor = image.GetSystemColor(color);

	const JCoordinate w = image.GetWidth();
	const JCoordinate h = image.GetHeight();
	for (JCoordinate y=0; y<h; y++)
		{
		for (JCoordinate x=0; x<w; x++)
			{
			if (image.GetSystemColor(x,y) == sysColor)
				{
				RemovePixel(x,y);
				}
			}
		}
}
