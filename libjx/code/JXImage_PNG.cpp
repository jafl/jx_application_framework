/******************************************************************************
 JXImage.cpp

	PNG functions separated out to allow smarter linking.

	Copyright (C) 1996-2009 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXImage.h"
#include "jx-af/jx/JXColorManager.h"
#include <jx-af/jcore/jAssert.h>

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
	assert( *image != nullptr );

	const JError err = (**image).ReadPNG(fileName);

	if (!err.OK())
	{
		jdelete *image;
		*image = nullptr;
	}
	return err;
}
