/******************************************************************************
 JXImageCache.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageCache
#define _H_JXImageCache

#include <jx-af/jcore/JStringPtrMap.h>
#include "jx-af/jcore/JXPM.h"

class JString;
class JXDisplay;
class JXImage;

class JXImageCache
{
public:

	JXImageCache(JXDisplay* display);

	virtual ~JXImageCache();

	JXImage*	GetImage(const JXPM& data);

private:

	JXDisplay*				itsDisplay;		// owns us
	JStringPtrMap<JXImage>*	itsMap;

private:

	JString	GetImageData(const JXPM& data) const;

	// not allowed

	JXImageCache(const JXImageCache&) = delete;
	JXImageCache& operator=(const JXImageCache&) = delete;
};

#endif
