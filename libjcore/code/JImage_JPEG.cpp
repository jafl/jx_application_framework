/******************************************************************************
 JImage.cpp

	PNG functions separated out to allow smarter linking.

	Copyright © 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JImage.h>
#include <JStdError.h>
#include <jAssert.h>

/******************************************************************************
 ReadJPEG (protected)

 ******************************************************************************/

JError
JImage::ReadJPEG
	(
	const JCharacter* fileName
	)
{
#ifdef _J_HAS_JPEG

	const JError err = ReadGD(fileName, gdImageCreateFromJpeg);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotJPEG(fileName);
		}
	else
		{
		return err;
		}

#else

	return JPEGNotAvailable();

#endif
}

/******************************************************************************
 WriteJPEG

	If quality is negative, you get the default tradeoff.  Otherwise,
	quality must lie in the range [0,95].

 ******************************************************************************/

#ifdef _J_HAS_JPEG

static int jQuality = -1;
static void imageJpeg(gdImagePtr im, FILE *out);

void
imageJpeg
	(
	gdImagePtr	im,
	FILE*		out
	)
{
	gdImageJpeg(im, out, jQuality);
}

#endif

JError
JImage::WriteJPEG
	(
	const JCharacter*	fileName,
	const JBoolean		interlace,
	const int			quality
	)
	const
{
#ifdef _J_HAS_JPEG

	jQuality = quality;
	return WriteGD(fileName, kJTrue, kJTrue, interlace, imageJpeg);

#else

	return JPEGNotAvailable();

#endif
}
