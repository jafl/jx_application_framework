/******************************************************************************
 ImageCache.cpp

	Stores a mapping of file to JXImage*.

	BASE CLASS = none

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageCache.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageCache::ImageCache
	(
	JXDisplay* display
	)
	:
	itsDisplay(display)
{
	itsMap = jnew JStringPtrMap<JXImage>(JPtrArrayT::kDeleteAll);
	assert( itsMap != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageCache::~ImageCache()
{
	jdelete itsMap;
}

/******************************************************************************
 GetImage

 ******************************************************************************/

bool
ImageCache::GetImage
	(
	const JString&	fullName,
	JXImage**		image
	)
{
	JString trueName;
	if (!JGetTrueName(fullName, &trueName))
	{
		*image = nullptr;
		return false;
	}

	if (!itsMap->GetItem(trueName, image))
	{
		const JError err = JXImage::CreateFromXPM(itsDisplay, trueName, image);
		if (!err.OK())
		{
			*image = nullptr;
			return false;
		}

		(**image).ConvertToRemoteStorage();	// expect to use as-is

		const bool ok = itsMap->SetNewItem(trueName, *image);
		assert( ok );
	}

	return true;
}
