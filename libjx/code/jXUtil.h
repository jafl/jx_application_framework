/******************************************************************************
 jXUtil.h

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXUtil
#define _H_jXUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>
#include <JRect.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class JXDisplay;
class JXImageMask;

XRectangle JXJToXRect(const JRect& rect);

inline JRect
JXXToJRect
	(
	const XRectangle& rect
	)
{
	return JRect(rect.y, rect.x, rect.y + rect.height, rect.x + rect.width);
}

Region		JXRectangleRegion(XRectangle* rectangle);
Region		JXRectangleRegion(const JRect& rect);
JRect		JXGetRegionBounds(Region region);
JBoolean	JXRegionIsRectangle(Region region, JRect* rect = NULL);

Region	JXCopyRegion(Region region);
void	JXIntersectRectWithRegion(XRectangle* rectangle, Region src_region,
								  Region dest_region);
void	JXSubtractRectFromRegion(Region src_region, XRectangle* rectangle,
								 Region dest_region);

Pixmap	JXIntersection(JXDisplay* display,
					   Region region, const JPoint& regionOffset,
					   const JXImageMask& pixmap, const JPoint& pixmapOffset,
					   const JRect& resultRect);
Pixmap	JXIntersection(JXDisplay* display,
					   const JXImageMask& p1, const JPoint& p1Offset,
					   const JXImageMask& p2, const JPoint& p2Offset,
					   const JRect& resultRect);

inline Pixmap
JXIntersection
	(
	JXDisplay*			display,
	const JXImageMask&	pixmap,
	const JPoint&		pixmapOffset,
	Region				region,
	const JPoint&		regionOffset,
	const JRect&		resultRect
	)
{
	return JXIntersection(display, region, regionOffset, pixmap, pixmapOffset, resultRect);
}

JString	JXPackStrings(const JPtrArray<JString>& strList,
					  const JCharacter* separator = "\0", const JSize sepLength = 1);
void	JXUnpackStrings(const JCharacter* data, const JSize length,
						JPtrArray<JString>* strList,
						const JCharacter* separator = "\0", const JSize sepLength = 1);

JString	JXPackFileNames(const JPtrArray<JString>& fileNameList);
void	JXUnpackFileNames(const JCharacter* data, const JSize length,
						  JPtrArray<JString>* fileNameList,
						  JPtrArray<JString>* urlList);
void	JXReportUnreachableHosts(const JPtrArray<JString>& urlList);

#endif
