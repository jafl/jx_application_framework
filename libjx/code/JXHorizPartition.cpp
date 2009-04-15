/******************************************************************************
 JXHorizPartition.cpp

	Maintains a set of compartments arranged horizontally.  The user can drag
	the regions between compartments to reallocate space.

	BASE CLASS = JXPartition

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHorizPartition.h>
#include <JXDragPainter.h>
#include <JXColormap.h>
#include <JXCursor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The second version is for use by derived classes who need to call
	CreateInitialCompartments() themselves in order to use their version of
	CreateCompartment().

 ******************************************************************************/

JXHorizPartition::JXHorizPartition
	(
	const JArray<JCoordinate>&	widths,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minWidths,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXPartition(widths, elasticIndex, minWidths,
				enclosure, hSizing, vSizing, x,y, w,h)
{
	JXHorizPartitionX();
	CreateInitialCompartments();
}

// protected

JXHorizPartition::JXHorizPartition
	(
	JXContainer*				enclosure,
	const JArray<JCoordinate>&	widths,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minWidths,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXPartition(widths, elasticIndex, minWidths,
				enclosure, hSizing, vSizing, x,y, w,h)
{
	JXHorizPartitionX();
}

// private

void
JXHorizPartition::JXHorizPartitionX()
{
	itsDragType = kInvalidDrag;
	SetDefaultCursor(JXGetDragVertLineCursor(GetDisplay()));
	itsDragAllLineCursor = JXGetDragAllVertLineCursor(GetDisplay());

	SetElasticSize();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHorizPartition::~JXHorizPartition()
{
}

/******************************************************************************
 GetTotalSize (virtual)

 ******************************************************************************/

JCoordinate
JXHorizPartition::GetTotalSize()
	const
{
	return GetApertureWidth();
}

/******************************************************************************
 CreateCompartment (virtual protected)

 ******************************************************************************/

JXWidgetSet*
JXHorizPartition::CreateCompartment
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	JXWidgetSet* compartment =
		new JXWidgetSet(this, kFixedLeft, kVElastic,
						position,0, size, GetApertureHeight());
	assert( compartment != NULL );
	return compartment;
}

/******************************************************************************
 SetCompartmentSizes (virtual protected)

	Adjust the width of each compartment.

 ******************************************************************************/

void
JXHorizPartition::SetCompartmentSizes()
{
	const JCoordinate h = GetApertureHeight();

	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate x = 0;
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		const JCoordinate w = GetCompartmentSize(i);
		assert( w >= JPartition::GetMinCompartmentSize(i) );

		JXContainer* compartment = GetCompartment(i);
		compartment->Place(x,0);
		compartment->SetSize(w,h);
		x += w + kDragRegionSize;
		}
}

/******************************************************************************
 Draw (virtual protected)

	Draw 2 vertical lines in each drag region.

 ******************************************************************************/

void
JXHorizPartition::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor((p.GetColormap())->GetGray60Color());

	const JRect ap       = GetAperture();
	const JCoordinate y1 = ap.top+1;
	const JCoordinate y2 = ap.bottom-2;

	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate x = 0;
	for (JIndex i=1; i<compartmentCount; i++)
		{
		x += GetCompartmentSize(i);
		p.Line(x+1, y1, x+1, y2);
		p.Line(x+3, y1, x+3, y2);
		x += kDragRegionSize;
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

	Any mouse click that we receive means start dragging.

 ******************************************************************************/

void
JXHorizPartition::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragType = kInvalidDrag;
	if (button != kJXLeftButton)
		{
		return;
		}

	if (modifiers.meta())
		{
		itsDragType = kDragAll;
		PrepareToDragAll(pt.x, &itsMinDragX, &itsMaxDragX);
		}
	else
		{
		itsDragType = kDragOne;
		PrepareToDrag(pt.x, &itsMinDragX, &itsMaxDragX);
		}

	JPainter* p = CreateDragInsidePainter();

	const JRect ap = GetAperture();
	p->Line(pt.x, ap.top, pt.x, ap.bottom);
	itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXHorizPartition::HandleMouseDrag
	(
	const JPoint&			origPt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType != kInvalidDrag)
		{
		JPoint pt = origPt;

		// keep compartment width larger than minimum

		if (pt.x < itsMinDragX)
			{
			pt.x = itsMinDragX;
			}
		else if (pt.x > itsMaxDragX)
			{
			pt.x = itsMaxDragX;
			}

		// check if we have moved

		if (pt.x != itsPrevPt.x)
			{
			JPainter* p = NULL;
			const JBoolean ok = GetDragPainter(&p);
			assert( ok );

			JRect ap = GetAperture();
			p->Line(itsPrevPt.x, ap.top, itsPrevPt.x, ap.bottom);
			p->Line(pt.x, ap.top, pt.x, ap.bottom);

			itsPrevPt = pt;
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	We set the column width to itsPrevPt because that is what the user saw.

 ******************************************************************************/

void
JXHorizPartition::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType != kInvalidDrag)
		{
		// erase the line

		JPainter* p = NULL;
		const JBoolean ok = GetDragPainter(&p);
		assert( ok );

		JRect ap = GetAperture();
		p->Line(itsPrevPt.x, ap.top, itsPrevPt.x, ap.bottom);

		DeleteDragPainter();

		// set the compartment widths

		if (itsDragType == kDragAll)
			{
			AdjustCompartmentsAfterDragAll(itsPrevPt.x);
			}
		else
			{
			assert( itsDragType == kDragOne );
			AdjustCompartmentsAfterDrag(itsPrevPt.x);
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
JXHorizPartition::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (modifiers.meta())
		{
		DisplayCursor(itsDragAllLineCursor);
		}
	else
		{
		JXPartition::AdjustCursor(pt, modifiers);
		}
}
