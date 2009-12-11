/******************************************************************************
 JXImage.cpp

	PNG functions separated out to allow smarter linking.

	Copyright © 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (PNG file) (static)

	Returns an error if the file does not contain a PNG.

 ******************************************************************************/

JError
JXImage::CreateFromPNG
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	*image = new JXImage(display, colormap);
	assert( *image != NULL );

	const JBoolean saveApprox = colormap->WillApproximateColors();
	const JBoolean savePre    = colormap->WillPreemptivelyApproximateColors();
	colormap->ShouldApproximateColors(allowApproxColors);
	colormap->ShouldPreemptivelyApproximateColors(kJTrue);

	const JError err = (**image).ReadPNG(fileName);

	colormap->ShouldApproximateColors(saveApprox);
	colormap->ShouldPreemptivelyApproximateColors(savePre);

	if (!err.OK())
		{
		delete *image;
		*image = NULL;
		}
	return err;
}
