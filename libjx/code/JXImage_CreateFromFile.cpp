/******************************************************************************
 JXImage.cpp

	Image format functions separated out to allow smarter linking.

	Copyright (C) 1996-2009 by John Lindal.

 ******************************************************************************/

#include "JXImage.h"
#include "JXColorManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor function (any file) (static)

	Returns an error if the file does not contain a recognizable image format.

 ******************************************************************************/

JError
JXImage::CreateFromFile
	(
	JXDisplay*		display,
	const JString&	fileName,
	JXImage**		image
	)
{
	const FileType type = GetFileType(fileName);
	if (type == kGIFType)
		{
		return CreateFromGIF(display, fileName, image);
		}
	else if (type == kPNGType)
		{
		return CreateFromPNG(display, fileName, image);
		}
	else if (type == kJPEGType)
		{
		return CreateFromJPEG(display, fileName, image);
		}
	else if (type == kXPMType)
		{
		return CreateFromXPM(display, fileName, image);
		}
	else
		{
		*image = nullptr;
		return UnknownFileType(fileName);
		}
}
