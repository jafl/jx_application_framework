/******************************************************************************
 ImageCache.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_ImageCache
#define _H_ImageCache

#include <jx-af/jcore/JStringPtrMap.h>

class JDirInfo;
class JXDisplay;
class JXImage;
class JXImageMenu;

class ImageCache
{
public:

	ImageCache(JXDisplay* display);

	virtual ~ImageCache();

	bool	GetImage(const JString& fullName, JXImage** image);
	void	BuildIconMenu(const JString& appPath, const JSize maxSize,
						  const JRegex* excludePattern, const bool includeEmptyIcon,
						  JXImageMenu* menu, JPtrArray<JString>* pathList);
	void	LoadIcons(JDirInfo* info, const JSize maxSize, const JRegex* excludePattern,
					  JXImageMenu* menu, JPtrArray<JString>* pathList);

	static bool		FindProjectRoot(const JString& path, JString* root);
	static JString		ConvertToFullName(const JString& projPath,
										  const JString& name, const bool isCore);
	static JString	PrintInclude(const JString& fullName, std::ostream& output);

private:

	JXDisplay*				itsDisplay;		// owns us
	JStringPtrMap<JXImage>*	itsMap;

private:

	// not allowed

	ImageCache(const ImageCache&) = delete;
	ImageCache& operator=(const ImageCache&) = delete;
};

#endif
