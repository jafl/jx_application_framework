/******************************************************************************
 jXUtil.h

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXUtil
#define _H_jXUtil

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
bool	JXRegionIsRectangle(Region region, JRect* rect = nullptr);

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
					  const JUtf8Byte* separator = "\1", const JSize sepLength = 1);
void	JXUnpackStrings(const JUtf8Byte* data, const JSize byteCount,
						JPtrArray<JString>* strList,
						const JUtf8Byte* separator = "\1", const JSize sepLength = 1);

JString	JXPackFileNames(const JPtrArray<JString>& fileNameList);
void	JXUnpackFileNames(const JUtf8Byte* data, const JSize byteCount,
						  JPtrArray<JString>* fileNameList,
						  JPtrArray<JString>* urlList);
void	JXReportUnreachableHosts(const JPtrArray<JString>& urlList);

// only for use by JXSelectionManager

bool	JXFixBrokenURLs(const JUtf8Byte* data, const JSize byteCount,
							JXDisplay* display, const Window srcWindow,
							JString* newData);

#endif
