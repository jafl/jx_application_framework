/******************************************************************************
 jXUtil.cpp

	Copyright © 1996-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <jXUtil.h>
#include <JXDisplay.h>
#include <JXImageMask.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUnreachableHostsID = "UnreachableHosts::jXUtil";

/******************************************************************************
 JXJToXRect

 ******************************************************************************/

XRectangle
JXJToXRect
	(
	const JRect& rect
	)
{
	XRectangle xRect;
	xRect.x      = rect.left;
	xRect.y      = rect.top;
	xRect.width  = rect.width();
	xRect.height = rect.height();
	return xRect;
}

/******************************************************************************
 JXRectangleRegion

 ******************************************************************************/

Region
JXRectangleRegion
	(
	XRectangle* r
	)
{
	XPoint xpt[4] =
	{
		{r->x,            r->y},
		{r->x + r->width, r->y},
		{r->x + r->width, r->y + r->height},
		{r->x,            r->y + r->height}
	};
	return XPolygonRegion(xpt, 4, EvenOddRule);
}

Region
JXRectangleRegion
	(
	const JRect& r
	)
{
	XPoint xpt[4] =
	{
		{r.left,  r.top},
		{r.right, r.top},
		{r.right, r.bottom},
		{r.left,  r.bottom}
	};
	return XPolygonRegion(xpt, 4, EvenOddRule);
}

/******************************************************************************
 JXGetRegionBounds

	If the region is a rectangle and rect != NULL, *rect contains the rectangle.

 ******************************************************************************/

JRect
JXGetRegionBounds
	(
	Region region
	)
{
	XRectangle xRect;
	XClipBox(region, &xRect);
	return JXXToJRect(xRect);
}

/******************************************************************************
 JXRegionIsRectangle

	If the region is a rectangle and rect != NULL, *rect contains the rectangle.

 ******************************************************************************/

JBoolean
JXRegionIsRectangle
	(
	Region	region,
	JRect*	rect
	)
{
	XRectangle xRect;
	XClipBox(region, &xRect);
	if (XRectInRegion(region, xRect.x, xRect.y, xRect.width, xRect.height) ==
		RectangleIn)
		{
		if (rect != NULL)
			{
			*rect = JXXToJRect(xRect);
			}
		return kJTrue;
		}
	else
		{
		if (rect != NULL)
			{
			*rect = JRect(0,0,0,0);
			}
		return kJFalse;
		}
}

/******************************************************************************
 JXCopyRegion

	XUnionRectWithRegion would probably be faster, but it doesn't work
	with an empty rectangle.

 ******************************************************************************/

Region
JXCopyRegion
	(
	Region region
	)
{
	Region empty  = XCreateRegion();
	Region result = XCreateRegion();
	XUnionRegion(empty, region, result);
	XDestroyRegion(empty);
	return result;
}

/******************************************************************************
 JXIntersectRectWithRegion

	dest_region can be the same as src_region

 ******************************************************************************/

void
JXIntersectRectWithRegion
	(
	XRectangle*	rectangle,
	Region		src_region,
	Region		dest_region
	)
{
	Region r1 = JXRectangleRegion(rectangle);
	Region r2 = JXCopyRegion(src_region);
	XIntersectRegion(r1, r2, dest_region);
	XDestroyRegion(r1);
	XDestroyRegion(r2);
}

/******************************************************************************
 JXSubtractRectFromRegion

	dest_region can be the same as src_region

 ******************************************************************************/

void
JXSubtractRectFromRegion
	(
	Region		src_region,
	XRectangle*	rectangle,
	Region		dest_region
	)
{
	Region r1 = JXCopyRegion(src_region);
	Region r2 = JXRectangleRegion(rectangle);
	XSubtractRegion(r1, r2, dest_region);
	XDestroyRegion(r1);
	XDestroyRegion(r2);
}

/******************************************************************************
 JXIntersection

 ******************************************************************************/

Pixmap
JXIntersection
	(
	JXDisplay*			display,
	Region				region,
	const JPoint&		regionOffset,
	const JXImageMask&	p,
	const JPoint&		pixmapOffset,
	const JRect&		resultRect
	)
{
	JBoolean pixmapInsideRegion = kJFalse;
	JRect regionRect;
	if (JXRegionIsRectangle(region, &regionRect))
		{
		regionRect.Shift(regionOffset);
		JRect pixmapRect = p.GetBounds();
		pixmapRect.Shift(pixmapOffset);
		pixmapInsideRegion = regionRect.Contains(pixmapRect);
		}

	if (pixmapInsideRegion && pixmapOffset.x == 0 && pixmapOffset.y == 0)
		{
		// When the pixmap is already in the right position, we can save
		// a lot of time.

		return p.CreatePixmap();
		}

	const JCoordinate resultW = resultRect.width();
	const JCoordinate resultH = resultRect.height();
	Pixmap result = XCreatePixmap(*display, display->GetRootWindow(),
								  resultW, resultH, 1);
	assert( result != None );

	Pixmap pixmap = p.CreatePixmap();

	XGCValues values;
	GC tempGC = XCreateGC(*display, result, 0L, &values);
	XFillRectangle(*display, result, tempGC, 0,0, resultW, resultH);

	if (!pixmapInsideRegion)
		{
		// This is only necessary if the pixmap is not entirely inside the region.

		XSetRegion(*display, tempGC, region);
		XSetClipOrigin(*display, tempGC, regionOffset.x, regionOffset.y);
		}

	XCopyArea(*display, pixmap, result, tempGC,
			  0,0, p.GetWidth(), p.GetHeight(), pixmapOffset.x, pixmapOffset.y);
	XFreeGC(*display, tempGC);

	XFreePixmap(*display, pixmap);

	return result;
}

/******************************************************************************
 JXIntersection

 ******************************************************************************/

Pixmap
JXIntersection
	(
	JXDisplay*			display,
	const JXImageMask&	pix1,
	const JPoint&		p1Offset,
	const JXImageMask&	pix2,
	const JPoint&		p2Offset,
	const JRect&		resultRect
	)
{
	const JCoordinate resultW = resultRect.width();
	const JCoordinate resultH = resultRect.height();
	Pixmap result = XCreatePixmap(*display, display->GetRootWindow(),
								  resultW, resultH, 1);
	assert( result != None );

	Pixmap p1 = pix1.CreatePixmap();
	Pixmap p2 = pix2.CreatePixmap();

	XGCValues values;
	GC tempGC = XCreateGC(*display, result, 0L, &values);
	XFillRectangle(*display, result, tempGC, 0,0, resultW, resultH);
	XSetClipMask(*display, tempGC, p1);
	XSetClipOrigin(*display, tempGC, p1Offset.x, p1Offset.y);
	XCopyArea(*display, p2, result, tempGC,
			  0,0, pix2.GetWidth(), pix2.GetHeight(), p2Offset.x, p2Offset.y);
	XFreeGC(*display, tempGC);

	XFreePixmap(*display, p1);
	XFreePixmap(*display, p2);

	return result;
}

/******************************************************************************
 Packing strings

	These routines pack and unpack a list of strings using the standard
	X format of NULL separation.

	JXUnpackStrings() does not clear strList.

 ******************************************************************************/

JString
JXPackStrings
	(
	const JPtrArray<JString>&	strList,
	const JCharacter*			separator,
	const JSize					sepLength
	)
{
	JString data;

	const JSize count = strList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (i > 1)
			{
			data.Append(separator, sepLength);
			}
		data.Append(*(strList.NthElement(i)));
		}

	return data;
}

void
JXUnpackStrings
	(
	const JCharacter*	origData,
	const JSize			length,
	JPtrArray<JString>*	strList,
	const JCharacter*	separator,
	const JSize			sepLength
	)
{
	const JString data(origData, length);
	JIndex prevIndex = 1, i = 1;
	while (1)
		{
		const JBoolean found = data.LocateNextSubstring(separator, sepLength, &i);

		JString* str = new JString(origData + prevIndex-1, i - prevIndex);
		assert( str != NULL );
		strList->Append(str);

		if (found)
			{
			i += sepLength;		// move past the separator
			prevIndex = i;
			}
		else
			{
			break;
			}
		}
}

/******************************************************************************
 Packing file names

	These routines pack and unpack a list of file names using the text/uri-list
	format of URLs separated by CRLFs.

	JXUnpackFileNames() does not clear the output lists.  urlList contains
	the URLs that could not be converted to file names, if any.

 ******************************************************************************/

static const JCharacter* kURISeparator = "\r\n";
const JSize kURISeparatorLength        = 2;

const JCharacter kURICommentMarker = '#';

JString
JXPackFileNames
	(
	const JPtrArray<JString>& fileNameList
	)
{
	JString data;

	const JSize count = fileNameList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (i > 1)
			{
			data.Append(kURISeparator, kURISeparatorLength);
			}
		data.Append(JFileNameToURL(*(fileNameList.NthElement(i))));
		}

	return data;
}

void
JXUnpackFileNames
	(
	const JCharacter*	data,
	const JSize			length,
	JPtrArray<JString>*	fileNameList,
	JPtrArray<JString>*	urlList
	)
{
	const JSize origCount = fileNameList->GetElementCount();
	JXUnpackStrings(data, length, fileNameList, kURISeparator, kURISeparatorLength);
	const JSize newCount = fileNameList->GetElementCount();

	JString fileName;
	for (JIndex i=newCount; i>origCount; i--)
		{
		const JString* url = fileNameList->NthElement(i);
		if (url->IsEmpty() || url->GetFirstCharacter() == kURICommentMarker)
			{
			fileNameList->DeleteElement(i);
			}
		else if (JURLToFileName(*url, &fileName))
			{
			*(fileNameList->NthElement(i)) = fileName;
			}
		else
			{
			urlList->Append(fileNameList->NthElement(i));
			fileNameList->RemoveElement(i);
			}
		}
}

/******************************************************************************
 JXReportUnreachableHosts

 ******************************************************************************/

static const JRegex urlPattern = "://([^/]+)/";

void
JXReportUnreachableHosts
	(
	const JPtrArray<JString>& urlList
	)
{
	if (urlList.IsEmpty())
		{
		return;
		}

	JPtrArray<JString> hostList(JPtrArrayT::kDeleteAll);
	hostList.SetSortOrder(JOrderedSetT::kSortAscending);
	hostList.SetCompareFunction(JCompareStringsCaseInsensitive);

	const JSize urlCount = urlList.GetElementCount();
	JArray<JIndexRange> matchList;
	JString host;
	for (JIndex i=1; i<=urlCount; i++)
		{
		const JString* url = urlList.NthElement(i);
		if (urlPattern.Match(*url, &matchList))
			{
			JString* host = new JString(url->GetSubstring(matchList.GetElement(2)));
			assert( host != NULL );
			if (!hostList.InsertSorted(host, kJFalse))
				{
				delete host;
				}
			}
		}

	if (!hostList.IsEmpty())
		{
		JString hosts;
		const JSize hostCount = hostList.GetElementCount();
		for (JIndex i=1; i<=hostCount; i++)
			{
			if (!hosts.IsEmpty())
				{
				hosts.AppendCharacter('\n');
				}
			hosts += *(hostList.NthElement(i));
			}

		(JGetStringManager())->ReportError(kUnreachableHostsID, hosts);
		}
}
