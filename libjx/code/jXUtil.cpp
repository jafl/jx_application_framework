/******************************************************************************
 jXUtil.cpp

	Copyright (C) 1996-2001 by John Lindal.

 ******************************************************************************/

#include "jXUtil.h"
#include "JXDisplay.h"
#include "JXImageMask.h"
#include <X11/Xatom.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/JSubstitute.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// string ID's

static const JUtf8Byte* kUnreachableHostsID = "UnreachableHosts::jXUtil";

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

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

#pragma GCC diagnostic pop

/******************************************************************************
 JXGetRegionBounds

	If the region is a rectangle and rect != nullptr, *rect contains the rectangle.

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

	If the region is a rectangle and rect != nullptr, *rect contains the rectangle.

 ******************************************************************************/

bool
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
		if (rect != nullptr)
		{
			*rect = JXXToJRect(xRect);
		}
		return true;
	}
	else
	{
		if (rect != nullptr)
		{
			*rect = JRect(0,0,0,0);
		}
		return false;
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
	bool pixmapInsideRegion = false;
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
	X format of nullptr separation.

	JXUnpackStrings() does not clear strList.

 ******************************************************************************/

JString
JXPackStrings
	(
	const JPtrArray<JString>&	strList,
	const JUtf8Byte*			separator,
	const JSize					sepLength
	)
{
	const JString s(separator, sepLength, JString::kNoCopy);
	return JStringJoin(s, strList);
}

void
JXUnpackStrings
	(
	const JUtf8Byte*	origData,
	const JSize			byteCount,
	JPtrArray<JString>*	strList,
	const JUtf8Byte*	separator,
	const JSize			sepLength
	)
{
	JString(origData, byteCount, JString::kNoCopy).
		Split(
			JString(separator, sepLength, JString::kNoCopy), strList);
}

/******************************************************************************
 Packing file names

	These routines pack and unpack a list of file names using the text/uri-list
	format of URLs separated by CRLFs.

	JXUnpackFileNames() does not clear the output lists.  urlList contains
	the URLs that could not be converted to file names, if any.

 ******************************************************************************/

static const JUtf8Byte* kURISeparator = "\r\n";
const JSize kURISeparatorLength       = 2;

const JUtf8Byte kURICommentMarker = '#';

JString
JXPackFileNames
	(
	const JPtrArray<JString>& fileNameList
	)
{
	JString data;

	bool first = true;
	for (auto* name : fileNameList)
	{
		if (!first)
		{
			data.Append(kURISeparator, kURISeparatorLength);
		}
		data.Append(JFileNameToURL(*name));
		first = false;
	}

	return data;
}

void
JXUnpackFileNames
	(
	const JUtf8Byte*	data,
	const JSize			byteCount,
	JPtrArray<JString>*	fileNameList,
	JPtrArray<JString>*	urlList
	)
{
	const JSize origCount = fileNameList->GetItemCount();
	JXUnpackStrings(data, byteCount, fileNameList, kURISeparator, kURISeparatorLength);
	const JSize newCount = fileNameList->GetItemCount();

	JString fileName;
	for (JIndex i=newCount; i>origCount; i--)
	{
		const JString* url = fileNameList->GetItem(i);
		if (url->IsEmpty() || url->GetFirstCharacter() == kURICommentMarker)
		{
			fileNameList->DeleteItem(i);
		}
		else if (JURLToFileName(*url, &fileName))
		{
			*(fileNameList->GetItem(i)) = fileName;
		}
		else
		{
			urlList->Append(fileNameList->GetItem(i));
			fileNameList->RemoveItem(i);
		}
	}
}

/******************************************************************************
 JXReportUnreachableHosts

 ******************************************************************************/

static const JRegex urlPattern("://(.+?)/");

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
	hostList.SetSortOrder(JListT::kSortAscending);
	hostList.SetCompareFunction(JCompareStringsCaseInsensitive);

	for (auto* url : urlList)
	{
		JStringIterator iter(*url);
		if (iter.Next(urlPattern))
		{
			auto* host = jnew JString(iter.GetLastMatch().GetSubstring(1));
			if (!hostList.InsertSorted(host, false))
			{
				jdelete host;
			}
		}
	}

	if (!hostList.IsEmpty())
	{
		JString hosts;

		bool first = true;
		for (auto* host : hostList)
		{
			if (!first)
			{
				hosts.Append("\n");
			}
			hosts += *host;
			first  = false;
		}

		JGetStringManager()->ReportError(kUnreachableHostsID, hosts);
	}
}

/******************************************************************************
 JXFixBrokenURLs

 ******************************************************************************/

static const JUtf8Byte* kInvalidURLHostMarker = ":///";
static const JUtf8Byte* kURLHostPattern       = "://$host/";

static JString
jXGetClientMachineName
	(
	JXDisplay*		display,
	const Window	xWindow
	)
{
	JString machineName;

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* xdata;
	XGetWindowProperty(*display, xWindow, display->GetWMClientMachineXAtom(),
					   0, LONG_MAX, False, XA_STRING,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &xdata);
	if (actualType == XA_STRING && actualFormat == 8 && itemCount > 0)
	{
		machineName.Set((JUtf8Byte*) xdata, itemCount);
	}

	XFree(xdata);
	return machineName;
}

bool
JXFixBrokenURLs
	(
	const JUtf8Byte*	data,
	const JSize			byteCount,
	JXDisplay*			display,
	const Window		srcWindow,
	JString*			newData
	)
{
	JPtrArray<JString> urlList(JPtrArrayT::kDeleteAll);
	JXUnpackStrings(data, byteCount, &urlList, kURISeparator, kURISeparatorLength);

	bool changed = false;

	JString srcHost, tmp;
	for (auto* url : urlList)
	{
		JStringIterator iter(url);
		if (!url->IsEmpty() && url->GetFirstCharacter() != kURICommentMarker &&
			iter.Next(kInvalidURLHostMarker))
		{
			if (srcHost.IsEmpty())
			{
				srcHost = jXGetClientMachineName(display, srcWindow);
				if (srcHost.IsEmpty())
				{
					return false;
				}
			}

			tmp = kURLHostPattern;

			JSubstitute sub;
			sub.DefineVariable("host", srcHost);
			sub.Substitute(&tmp);

			iter.ReplaceLastMatch(tmp);
			changed = true;
		}
	}

	if (changed)
	{
		*newData = JXPackStrings(urlList, kURISeparator, kURISeparatorLength);
	}
	else
	{
		newData->Clear();
	}

	return changed;
}
