/******************************************************************************
 JXImage.cpp

	Image format functions separated out to allow smarter linking.

	Copyright © 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (any file) (static)

	Returns an error if the file does not contain a recognizable image format.

 ******************************************************************************/

JError
JXImage::CreateFromFile
	(
	JXDisplay*			display,
	JXColormap*			colormap,
	const JCharacter*	fileName,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	const FileType type = GetFileType(fileName);
	if (type == kGIFType)
		{
		return CreateFromGIF(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kPNGType)
		{
		return CreateFromPNG(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kJPEGType)
		{
		return CreateFromJPEG(display, colormap, fileName, image, allowApproxColors);
		}
	else if (type == kXPMType)
		{
		return CreateFromXPM(display, colormap, fileName, image, allowApproxColors);
		}
	else
		{
		*image = NULL;
		return UnknownFileType(fileName);
		}
}
