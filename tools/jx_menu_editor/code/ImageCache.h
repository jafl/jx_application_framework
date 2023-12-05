/******************************************************************************
 ImageCache.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageCache
#define _H_ImageCache

#include <jx-af/jcore/JStringPtrMap.h>

class JString;
class JXDisplay;
class JXImage;

class ImageCache
{
public:

	ImageCache(JXDisplay* display);

	virtual ~ImageCache();

	bool	GetImage(const JString& fullName, JXImage** image);

private:

	JXDisplay*				itsDisplay;		// owns us
	JStringPtrMap<JXImage>*	itsMap;

private:

	// not allowed

	ImageCache(const ImageCache&) = delete;
	ImageCache& operator=(const ImageCache&) = delete;
};

#endif
