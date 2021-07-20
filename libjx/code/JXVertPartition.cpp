/******************************************************************************
 JXVertPartition.cpp

	Maintains a set of compartments arranged vertically.  The user can drag
	the regions between compartments to reallocate space.

	BASE CLASS = JXPartition

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXVertPartition.h"
#include "JXRestorePartitionGeometry.h"
#include "JXDragPainter.h"
#include "JXColorManager.h"
#include "JXCursor.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

	The second version is for use by derived classes who need to call
	CreateInitialCompartments() themselves in order to use their version of
	CreateCompartment().

 ******************************************************************************/

JXVertPartition::JXVertPartition
	(
	const JArray<JCoordinate>&	heights,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minHeights,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXPartition(heights, elasticIndex, minHeights,
				enclosure, hSizing, vSizing, x,y, w,h)
{
	JXVertPartitionX();
	CreateInitialCompartments();
}

// protected

JXVertPartition::JXVertPartition
	(
	JXContainer*				enclosure,
	const JArray<JCoordinate>&	heights,
	const JIndex				elasticIndex,
	const JArray<JCoordinate>&	minHeights,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXPartition(heights, elasticIndex, minHeights,
				enclosure, hSizing, vSizing, x,y, w,h)
{
	JXVertPartitionX();
}

// private

void
JXVertPartition::JXVertPartitionX()
{
	itsDragType = kInvalidDrag;
	SetDefaultCursor(JXGetDragHorizLineCursor(GetDisplay()));
	itsDragAllLineCursor = JXGetDragAllHorizLineCursor(GetDisplay());

	itsFTCSizes    = nullptr;
	itsFTCMinSizes = nullptr;
	itsSavedGeom   = nullptr;

	SetElasticSize();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXVertPartition::~JXVertPartition()
{
	jdelete itsFTCSizes;
	jdelete itsFTCMinSizes;
	jdelete itsSavedGeom;
}

/******************************************************************************
 GetTotalSize (virtual)

 ******************************************************************************/

JCoordinate
JXVertPartition::GetTotalSize()
	const
{
	return GetApertureHeight();
}

/******************************************************************************
 CreateCompartment (virtual protected)

 ******************************************************************************/

JXWidgetSet*
JXVertPartition::CreateCompartment
	(
	const JIndex		index,
	const JCoordinate	position,
	const JCoordinate	size
	)
{
	auto* compartment =
		jnew JXWidgetSet(this, kHElastic, kFixedTop,
						0,position, GetApertureWidth(), size);
	assert( compartment != nullptr );
	compartment->SetNeedsInternalFTC();
	return compartment;
}

/******************************************************************************
 UpdateCompartmentSizes (virtual protected)

	Adjust the width of each compartment.

 ******************************************************************************/

void
JXVertPartition::UpdateCompartmentSizes()
{
	const JCoordinate w = GetApertureWidth();

	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate y = 0;
	for (JIndex i=1; i<=compartmentCount; i++)
		{
		const JCoordinate h = GetCompartmentSize(i);
		assert( h >= JPartition::GetMinCompartmentSize(i) );

		JXContainer* compartment = GetCompartment(i);
		compartment->Place(0,y);
		compartment->SetSize(w,h);
		y += h + kDragRegionSize;
		}
}

/******************************************************************************
 Draw (virtual protected)

	Draw 2 horizontal lines in each drag region.

 ******************************************************************************/

void
JXVertPartition::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor(JColorManager::GetGrayColor(60));

	const JRect ap       = GetAperture();
	const JCoordinate x1 = ap.left+1;
	const JCoordinate x2 = ap.right-2;

	const JSize compartmentCount = GetCompartmentCount();
	JCoordinate y = 0;
	for (JIndex i=1; i<compartmentCount; i++)
		{
		y += GetCompartmentSize(i);
		p.Line(x1, y+1, x2, y+1);
		p.Line(x1, y+3, x2, y+3);
		y += kDragRegionSize;
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

	Any mouse click that we receive means start dragging.

 ******************************************************************************/

void
JXVertPartition::HandleMouseDown
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
		PrepareToDragAll(pt.y, &itsMinDragY, &itsMaxDragY);
		}
	else
		{
		itsDragType = kDragOne;
		PrepareToDrag(pt.y, &itsMinDragY, &itsMaxDragY);
		}

	JPainter* p = CreateDragInsidePainter();

	const JRect ap = GetAperture();
	p->Line(ap.left, pt.y, ap.right, pt.y);
	itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXVertPartition::HandleMouseDrag
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

		if (pt.y < itsMinDragY)
			{
			pt.y = itsMinDragY;
			}
		else if (pt.y > itsMaxDragY)
			{
			pt.y = itsMaxDragY;
			}

		// check if we have moved

		if (pt.y != itsPrevPt.y)
			{
			JPainter* p = nullptr;
			const bool ok = GetDragPainter(&p);
			assert( ok );

			JRect ap = GetAperture();
			p->Line(ap.left, itsPrevPt.y, ap.right, itsPrevPt.y);
			p->Line(ap.left, pt.y, ap.right, pt.y);

			itsPrevPt = pt;
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	We set the column width to itsPrevPt because that is what the user saw.

 ******************************************************************************/

void
JXVertPartition::HandleMouseUp
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

		JPainter* p = nullptr;
		const bool ok = GetDragPainter(&p);
		assert( ok );

		JRect ap = GetAperture();
		p->Line(ap.left, itsPrevPt.y, ap.right, itsPrevPt.y);

		DeleteDragPainter();

		// set the compartment widths

		if (itsDragType == kDragAll)
			{
			AdjustCompartmentsAfterDragAll(itsPrevPt.y);
			}
		else
			{
			assert( itsDragType == kDragOne );
			AdjustCompartmentsAfterDrag(itsPrevPt.y);
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
JXVertPartition::AdjustCursor
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

/******************************************************************************
 RunInternalFTC (virtual protected)

	Expand and return new size.

 ******************************************************************************/

bool
JXVertPartition::RunInternalFTC
	(
	const bool	horizontal,
	JCoordinate*	newSize
	)
{
	JPtrArrayIterator<JXContainer> iter(GetCompartments());
	JXContainer* obj;

	if (horizontal)
		{
		JCoordinate w = 0;
		while (iter.Next(&obj))
			{
			JCoordinate w1;
			if (obj->RunInternalFTC(true, &w1))
				{
				obj->FTCAdjustSize(w1 - obj->GetApertureWidth(), 0);
				}
			else
				{
				w1 = obj->GetApertureWidth();
				}

			w = JMax(w, w1);
			}

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		while (iter.Next(&obj))
			{
			obj->AdjustSize(w - obj->GetApertureWidth(), 0);
			}

		*newSize = w + 2 * GetBorderWidth();
		}
	else	// vertical
		{
		itsFTCSizes = jnew JArray<JCoordinate>;
		assert( itsFTCSizes != nullptr );

		itsFTCMinSizes = jnew JArray<JCoordinate>;
		assert( itsFTCMinSizes != nullptr );

		JCoordinate sum = 0;
		JIndex i        = 1;
		while (iter.Next(&obj))
			{
			JCoordinate h, delta;
			const JRect padding = obj->ComputePaddingForInternalFTC();
			if (obj->RunInternalFTC(false, &h))
				{
				h    += padding.top + padding.bottom;
				delta = h - obj->GetApertureHeight();
				obj->FTCAdjustSize(0, delta);
				}
			else
				{
				h     = GetCompartmentSize(i);
				delta = 0;
				}

			itsFTCSizes->AppendElement(h);
			itsFTCMinSizes->AppendElement(JPartition::GetMinCompartmentSize(i) + delta);
			sum += h;
			i++;
			}

		*newSize = sum + (itsFTCSizes->GetElementCount() - 1) * kDragRegionSize;
		}

	return true;
}

/******************************************************************************
 FTCAdjustSize (virtual protected)

 ******************************************************************************/

void
JXVertPartition::FTCAdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXPartition::FTCAdjustSize(dw, dh);

	if (itsFTCSizes != nullptr)
		{
		SetCompartmentSizes(*itsFTCSizes);
		jdelete itsFTCSizes;
		itsFTCSizes = nullptr;

		SetMinCompartmentSizes(*itsFTCMinSizes);
		jdelete itsFTCMinSizes;
		itsFTCMinSizes = nullptr;

		if (itsSavedGeom != nullptr)
			{
			RestoreGeometry(*itsSavedGeom);
			jdelete itsSavedGeom;
			itsSavedGeom = nullptr;
			}
		}
}

/******************************************************************************
 SaveGeometryForLater (virtual protected)

 ******************************************************************************/

bool
JXVertPartition::SaveGeometryForLater
	(
	const JArray<JCoordinate>& sizes
	)
{
	if (itsSavedGeom == nullptr)
		{
		itsSavedGeom = jnew JArray<JCoordinate>(sizes);
		assert( itsSavedGeom != nullptr );

		JXUrgentTask* geomTask = jnew JXRestorePartitionGeometry(this);
		assert( geomTask != nullptr );
		geomTask->Go();
		}
	else
		{
		*itsSavedGeom = sizes;
		}

	return true;
}
