/******************************************************************************
 JXHorizPartition.cpp

	Maintains a set of compartments arranged horizontally.  The user can drag
	the regions between compartments to reallocate space.

	BASE CLASS = JXPartition

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXHorizPartition.h"
#include "JXRestorePartitionGeometry.h"
#include "JXDragPainter.h"
#include "JXCursor.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

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

	itsFTCSizes    = nullptr;
	itsFTCMinSizes = nullptr;
	itsSavedGeom   = nullptr;

	SetElasticSize();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHorizPartition::~JXHorizPartition()
{
	jdelete itsFTCSizes;
	jdelete itsFTCMinSizes;
	jdelete itsSavedGeom;
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
	auto* compartment =
		jnew JXWidgetSet(this, kFixedLeft, kVElastic,
						position,0, size, GetApertureHeight());
	assert( compartment != nullptr );
	compartment->SetNeedsInternalFTC();
	return compartment;
}

/******************************************************************************
 UpdateCompartmentSizes (virtual protected)

	Adjust the width of each compartment.

 ******************************************************************************/

void
JXHorizPartition::UpdateCompartmentSizes()
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
	p.SetPenColor(JColorManager::GetGrayColor(60));

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
			JPainter* p = nullptr;
			const bool ok = GetDragPainter(&p);
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

		JPainter* p = nullptr;
		const bool ok = GetDragPainter(&p);
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

/******************************************************************************
 RunInternalFTC (virtual protected)

	Expand and return new size.

 ******************************************************************************/

bool
JXHorizPartition::RunInternalFTC
	(
	const bool		horizontal,
	JCoordinate*	newSize
	)
{
	JPtrArrayIterator<JXContainer> iter(GetCompartments());
	JXContainer* obj;

	if (horizontal)
	{
		itsFTCSizes = jnew JArray<JCoordinate>;
		assert( itsFTCSizes != nullptr );

		itsFTCMinSizes = jnew JArray<JCoordinate>;
		assert( itsFTCMinSizes != nullptr );

		JCoordinate sum = 0;
		JIndex i        = 1;
		while (iter.Next(&obj))
		{
			JCoordinate w, delta;
			const JRect padding = obj->ComputePaddingForInternalFTC();
			if (obj->RunInternalFTC(true, &w))
			{
				w    += padding.left + padding.right;
				delta = w - obj->GetApertureWidth();
				obj->FTCAdjustSize(delta, 0);
			}
			else
			{
				w     = GetCompartmentSize(i);
				delta = 0;
			}

			itsFTCSizes->AppendElement(w);
			itsFTCMinSizes->AppendElement(JPartition::GetMinCompartmentSize(i) + delta);
			sum += w;
			i++;
		}

		*newSize = sum + (itsFTCSizes->GetElementCount() - 1) * kDragRegionSize;
	}
	else	// vertical
	{
		JCoordinate h = 0;
		while (iter.Next(&obj))
		{
			JCoordinate h1;
			if (obj->RunInternalFTC(false, &h1))
			{
				obj->FTCAdjustSize(0, h1 - obj->GetApertureHeight());
			}
			else
			{
				h1 = obj->GetApertureHeight();
			}

			h = JMax(h, h1);
		}

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		while (iter.Next(&obj))
		{
			obj->AdjustSize(0, h - obj->GetApertureHeight());
		}

		*newSize = h + 2 * GetBorderWidth();
	}

	return true;
}

/******************************************************************************
 FTCAdjustSize (virtual protected)

 ******************************************************************************/

void
JXHorizPartition::FTCAdjustSize
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
JXHorizPartition::SaveGeometryForLater
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
