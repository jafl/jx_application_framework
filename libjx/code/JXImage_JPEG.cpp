/******************************************************************************
 JXImage.cpp

	JPEG functions separated out to allow smarter linking.

	Copyright (C) 1996-2009 by John Lindal.

 ******************************************************************************/

#include "JXImage.h"
#include "JXColorManager.h"
#include <jAssert.h>

/******************************************************************************
 Constructor function (JPEG file) (static)

	Returns an error if the file does not contain a JPEG.

 ******************************************************************************/

JError
JXImage::CreateFromJPEG
	(
	JXDisplay*		display,
	const JString&	fileName,
	JXImage**		image
	)
{
	*image = jnew JXImage(display);
	assert( *image != nullptr );

	const JError err = (**image).ReadJPEG(fileName);

	if (!err.OK())
	{
		jdelete *image;
		*image = nullptr;
	}
	return err;
}
