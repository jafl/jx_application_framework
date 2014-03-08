/******************************************************************************
 JXImageCache.cpp

	Stores a mapping of JXPM data to JXImage*.

	BASE CLASS = none

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageCache.h>
#include <JXDisplay.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageCache::JXImageCache
	(
	JXDisplay* display
	)
	:
	itsDisplay(display)
{
	itsMap = new JStringPtrMap<JXImage>(JPtrArrayT::kDeleteAll);
	assert( itsMap != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageCache::~JXImageCache()
{
	delete itsMap;
}

/******************************************************************************
 GetImage

 ******************************************************************************/

JXImage*
JXImageCache::GetImage
	(
	const JXPM& data
	)
{
	const JString s = GetImageData(data);

	JXImage* image;
	if (!itsMap->GetElement(s, &image))
		{
		image = new JXImage(itsDisplay, data);
		assert( image != NULL );
		const JBoolean ok = itsMap->SetNewElement(s, image);
		assert( ok );
		}

	return image;
}

/******************************************************************************
 GetImageData (private)

 ******************************************************************************/

JString
JXImageCache::GetImageData
	(
	const JXPM& data
	)
	const
{
	JCoordinate w, h;
	JSize colorCount, imageCharSize;
	const int readCount = sscanf(data.xpm[0], "%ld %ld %lu %lu",
								 &w, &h, &colorCount, &imageCharSize);
	assert( readCount == 4 );

	JString s = data.xpm[0];

	const JSize count = colorCount + h;
	for (JIndex i=1; i<=count; i++)
		{
		s += data.xpm[i];
		}

	return s;
}
