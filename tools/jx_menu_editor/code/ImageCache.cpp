/******************************************************************************
 ImageCache.cpp

	Stores a mapping of file to JXImage*.

	BASE CLASS = none

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageCache.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JString coreIconPath(JX_INCLUDE_PATH "/image/jx");
static const JString localIconPath("image");
static const JString iconPattern("*.png *.gif *.xpm");

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
		const JError err = JXImage::CreateFromFile(itsDisplay, trueName, image);
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

/******************************************************************************
 BuildIconMenu (private)

 ******************************************************************************/

static const char * empty_icon[] = {
"1 1 1 1",
" 	c None",
" "};

void
ImageCache::BuildIconMenu
	(
	const JString&		appPath,
	const JSize			maxSize,
	const JRegex*		excludePattern,
	const bool			includeEmptyIcon,
	JXImageMenu*		menu,
	JPtrArray<JString>*	pathList
	)
{
	menu->RemoveAllItems();
	pathList->CleanOut();

	JDirInfo* info;

	if (!appPath.IsEmpty())
	{
		const JString iconPath = JCombinePathAndName(appPath, localIconPath);
		if (JDirInfo::Create(iconPath, &info))
		{
			LoadIcons(info, maxSize, excludePattern, menu, pathList);
			jdelete info;
		}
	}

	if (JDirInfo::Create(coreIconPath, &info))
	{
		LoadIcons(info, maxSize, excludePattern, menu, pathList);
		jdelete info;
	}

	if (includeEmptyIcon)
	{
		auto* image = jnew JXImage(menu->GetDisplay(), empty_icon);
		menu->PrependItem(image, true);
		menu->SetTitleImage(image, false);
	}
}

/******************************************************************************
 LoadIcons (private)

 ******************************************************************************/

void
ImageCache::LoadIcons
	(
	JDirInfo*			info,
	const JSize			maxSize,
	const JRegex*		excludePattern,
	JXImageMenu*		menu,
	JPtrArray<JString>*	pathList
	)
{
	info->ShowDirs(false);
	info->SetWildcardFilter(iconPattern);

	for (const auto* e : *info)
	{
		if (excludePattern != nullptr && excludePattern->Match(e->GetName()))
		{
			continue;
		}

		JXImage* image;
		if (GetImage(e->GetFullName(), &image) &&
			(maxSize == 0 ||
			 (image->GetWidth()  <= JCoordinate(maxSize) &&
			  image->GetHeight() <= JCoordinate(maxSize))))
		{
			menu->AppendItem(image, false);
			pathList->Append(e->GetFullName());
		}
	}
}

/******************************************************************************
 FindProjectRoot (static)

 ******************************************************************************/

bool
ImageCache::FindProjectRoot
	(
	const JString&	path,
	JString*		root
	)
{
	JString p = path, n;
	do
	{
		n = JCombinePathAndName(p, "Makefile");
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, "Make.header");
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, "CMakeLists.txt");
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		JSplitPathAndName(p, &p, &n);
	}
	while (!JIsRootDirectory(p));

	root->Clear();
	return false;
}

/******************************************************************************
 ConvertToFullName (static)

 ******************************************************************************/

JString
ImageCache::ConvertToFullName
	(
	const JString&	projPath,
	const JString&	name,
	const bool		isCore
	)
{
	if (isCore)
	{
		return JCombinePathAndName(coreIconPath, name);
	}
	else if (projPath.IsEmpty())
	{
		return projPath;
	}

	const JString p = JCombinePathAndName(projPath, localIconPath);
	return JCombinePathAndName(p, name);
}

/******************************************************************************
 PrintInclude (static)

 ******************************************************************************/

JString
ImageCache::PrintInclude
	(
	const JString&	fullName,
	std::ostream&	output
	)
{
	JString p,n,r;
	const bool isCoreIcon = fullName.StartsWith(coreIconPath);
	JSplitPathAndName(fullName, &p, &n);
	JSplitRootAndSuffix(n, &r, &p);

	p.Set(isCoreIcon ? "jx_af_image_jx_" : "");

	output << "#ifndef _H_";
	p.Print(output);
	r.Print(output);
	output << std::endl;

	output << "#define _H_";
	p.Print(output);
	r.Print(output);
	output << std::endl;

	output << "#include ";
	output << (isCoreIcon ? "<jx-af/image/jx/" : "\"");
	r.Print(output);
	output << ".xpm";
	output << (isCoreIcon ? '>' : '"') << std::endl;

	output << "#endif" << std::endl;

	return r;
}
