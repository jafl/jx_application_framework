/******************************************************************************
 JXImage.cpp

	PNG functions separated out to allow smarter linking.

	Copyright (C) 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

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
	JXDisplay*		display,
	const JString&	fileName,
	JXImage**		image
	)
{
	*image = jnew JXImage(display);
	assert( *image != NULL );

	const JError err = (**image).ReadPNG(fileName);

	if (!err.OK())
		{
		jdelete *image;
		*image = NULL;
		}
	return err;
}
