/******************************************************************************
 JExprRectList.cpp

	This class stores the rendering rectangles for a tree of JFunctions.
	We do not own the JFunctions.  We only store pointers to them to allow
	easy lookup.  We also store the logical midline for each rectangle because
	this is usually not the physical midline.

	BASE CLASS = JContainer

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JExprRectList.h>
#include <JFunction.h>
#include <JRect.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JExprRectList::JExprRectList()
	:
	JContainer()
{
	itsRects = new JArray<JRect>(10);
	assert( itsRects != NULL );

	itsExtraInfo = new JArray<ExtraInfo>(10);
	assert( itsExtraInfo != NULL );

	itsFunctions = new JPtrArray<JFunction>(JPtrArrayT::kForgetAll, 10);
	assert( itsFunctions != NULL );

	InstallOrderedSet(itsRects);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExprRectList::~JExprRectList()
{
	delete itsRects;
	delete itsExtraInfo;
	delete itsFunctions;
}

/******************************************************************************
 AddRect

 ******************************************************************************/

JIndex
JExprRectList::AddRect
	(
	const JRect&		rect,
	const JCoordinate	midline,
	const JSize			fontSize,
	JFunction*			f
	)
{
	itsRects->AppendElement(rect);

	ExtraInfo info;
	info.midline  = midline;
	info.fontSize = fontSize;
	itsExtraInfo->AppendElement(info);

	itsFunctions->Append(f);

	return GetElementCount();
}

/******************************************************************************
 ShiftRect

	When we shift a rectangle, we must also shift all rectangles that were
	enclosed by the given rectangle.

 ******************************************************************************/

void
JExprRectList::ShiftRect
	(
	const JIndex		index,
	const JCoordinate	dx,
	const JCoordinate	dy
	)
{
	if (dx == 0 && dy == 0)
		{
		return;
		}

	JRect rect = itsRects->GetElement(index);
	const JRect origRect = rect;
	rect.Shift(dx, dy);
	itsRects->SetElement(index, rect);

	ExtraInfo info = itsExtraInfo->GetElement(index);
	info.midline += dy;
	itsExtraInfo->SetElement(index, info);

	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		rect = itsRects->GetElement(i);
		if (origRect.Contains(rect))
			{
			rect.Shift(dx, dy);
			itsRects->SetElement(i, rect);

			info = itsExtraInfo->GetElement(i);
			info.midline += dy;
			itsExtraInfo->SetElement(i, info);
			}
		}
}

/******************************************************************************
 GetSelection

	Returns the index of the smallest rectangle that contains both
	startPt and currPt.  Returns 0 if there is no such rectangle.
	If startPt is inside the bounding rectangle and currPt is outside,
	we return the index of the bounding rectangle.

	SORT:

	This routine would be faster if the rects were sorted in increasing
	order by area.  Then we could just pick the first rect that contained
	both pt1 and pt2.  (Equal area rectangles don't overlap.)  The rects
	could be sorted either in AddRect or after PrepareToRender was finished.

	The problem with sorting the rects is that the indices will change,
	so nobody can assume they are constant.  This is minimized by sorting
	after PrepareToRender is finished.

 ******************************************************************************/

JIndex
JExprRectList::GetSelection
	(
	const JPoint&	startPt,
	const JPoint&	currPt
	)
	const
{
	// Check if startPt is in the bounding rect.

	const JRect boundsRect = GetBoundsRect();
	if (!boundsRect.Contains(startPt))
		{
		return 0;
		}

	// The bounding rect is the last rect in the list.

	const JSize rectCount = itsRects->GetElementCount();
	JSize minArea         = 0;
	JIndex result         = rectCount;
	for (JIndex i=1; i<rectCount; i++)
		{
		const JRect rect = itsRects->GetElement(i);
		const JSize area = rect.area();
		if (rect.Contains(startPt) && rect.Contains(currPt) &&
			(minArea == 0 || area < minArea))
			{
			result = i;
			minArea = area;
			}
		}

	return result;
}

/******************************************************************************
 GetParent

	Returns the index of the smallest rectangle enclosing the given one.

	SORT:

	If the rects are sorted, we know we only have to check later ones.
	We can take the first one that we find because larger enclosing rects
	come later in the list.

	If the rects aren't sorted, we can still check only the later ones
	because all the arguments are added before the parent.  This also insures
	that we can always choose the first one because we don't overlap other
	rects generated by our parent and we will hit our parent's rect before
	we hit our parent's parent's rect.

 ******************************************************************************/

JIndex
JExprRectList::GetParent
	(
	const JIndex index
	)
	const
{
	const JRect smallRect = itsRects->GetElement(index);

	const JSize rectCount = itsRects->GetElementCount();
	for (JIndex i=index+1; i<=rectCount; i++)
		{
		const JRect bigRect = itsRects->GetElement(i);
		if (bigRect.Contains(smallRect))
			{
			return i;
			}
		}

	// The last element is always the largest, and includes all others.

	return rectCount;
}
