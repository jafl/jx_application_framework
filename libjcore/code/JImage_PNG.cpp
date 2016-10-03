/******************************************************************************
 JImage.cpp

	PNG functions separated out to allow smarter linking.

	Copyright (C) 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JImage.h>
#include <JStdError.h>
#include <jAssert.h>

/******************************************************************************
 ReadPNG (protected)

 ******************************************************************************/

JError
JImage::ReadPNG
	(
	const JCharacter* fileName
	)
{
#ifdef _J_HAS_PNG

	const JError err = ReadGD(fileName, gdImageCreateFromPng);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotPNG(fileName);
		}
	else
		{
		return err;
		}

#else

	return PNGNotAvailable();

#endif
}

/******************************************************************************
 WritePNG

 ******************************************************************************/

JError
JImage::WritePNG
	(
	const JCharacter*	fileName,
	const JBoolean		useTrueColor,
	const JBoolean		compressColorsToFit,
	const JBoolean		interlace
	)
	const
{
#ifdef _J_HAS_PNG

	return WriteGD(fileName, useTrueColor, compressColorsToFit, interlace, gdImagePng);

#else

	return PNGNotAvailable();

#endif
}
