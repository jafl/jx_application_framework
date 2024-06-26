/******************************************************************************
 JXFTCCell.cpp

	Represents a cell in the nested tables constructed by
	JXContainer::ExpandToFitContent.

	BASE CLASS = JXContainer

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "JXFTCCell.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXWidget.h"
#include "JXDragPainter.h"
#include "JXDNDManager.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JListUtil.h>
#include <sstream>
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFTCCell::JXFTCCell
	(
	JXContainer*	matchObj,
	JXContainer*	enc,
	const Direction	direction,
	const bool	exact
	)
	:
	JXContainer(enc->GetWindow(), enc),
	itsWidget(matchObj),
	itsDirection(direction),
	itsIsExactFlag(exact),
	itsElasticFlag(kUnknown),
	itsSyncChildrenFlag(false),
	itsChildren(nullptr),
	itsChildSpacing(nullptr),
	itsChildPositions(nullptr)
{
	assert( itsWidget == nullptr || itsDirection == kNoDirection );

	if (itsWidget != nullptr)
	{
		itsFrameG = itsWidget->GetFrameForFTC();
	}
	else if (GetEnclosure() != GetWindow())
	{
		itsFrameG = GetEnclosure()->GetFrameGlobal();
	}

	Refresh();

	if (theDebugFTCFlag)
	{
		GetFTCLog() << "Creating " << ToString() << std::endl;
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFTCCell::~JXFTCCell()
{
	if (theDebugFTCFlag)
	{
		GetFTCLog() << "Deleting " << ToString() << std::endl;
	}

	JXWindow* window = GetWindow();
	if (IsVisible())
	{
		window->RefreshRect(itsFrameG);
	}

	jdelete itsChildren;
	jdelete itsChildSpacing;
	jdelete itsChildPositions;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
JXFTCCell::ToString()
	const
{
	std::ostringstream s;
	JXContainer::ToString().Print(s);
	s << GetDirectionName();
	if (itsWidget != nullptr)
	{
		s << " (";
		itsWidget->ToString().Print(s);
		s << ")";
	}
	return JString(s.str(), JUtf8ByteRange());
}

/******************************************************************************
 GetDirectionName (private)

 ******************************************************************************/

const JUtf8Byte*
JXFTCCell::GetDirectionName()
	const
{
	return (itsDirection == kHorizontal ? " (horiz)" :
			itsDirection == kVertical   ? " (vert)"  :
			"");
}

/******************************************************************************
 Indent (private)

 ******************************************************************************/

JString
JXFTCCell::Indent
	(
	const JSize extra
	)
	const
{
	JString s;
	const JSize count = GetDepth() + extra;
	for (JIndex i=1; i<=count; i++)
	{
		s += "  ";
	}
	return s;
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

JSize
JXFTCCell::GetDepth()
	const
{
	JSize depth = 0;

	JXContainer* p = GetEnclosure();
	while (p != nullptr)
	{
		depth++;
		p = p->GetEnclosure();
	}

	return depth - 1;
}

/******************************************************************************
 Expand

	Expand enclosed objects to fit their content and then enforce
	invariants on the result.  Returns the new size.

 ******************************************************************************/

JCoordinate
JXFTCCell::Expand
	(
	const bool horizontal
	)
{
	itsSyncChildrenFlag   = true;
	itsSyncHorizontalFlag = horizontal;

	if (itsWidget != nullptr)
	{
		return ExpandWidget();
	}

	assert( itsDirection != kNoDirection );

	if (theDebugFTCFlag && GetEnclosure() == GetWindow())
	{
		GetFTCLog() << "----------" << std::endl;
		GetFTCLog() << "ExpandLayout (" << (horizontal ? "horiz" : "vert") << "):" << std::endl;
	}
	else if (theDebugFTCFlag)
	{
		GetFTCLog() << Indent() << "Checking cell " << ToString() << std::endl;
	}

	BuildChildList();
	ComputeInvariants();

	for (auto* cell : *itsChildren)
	{
		cell->Expand(horizontal);
	}

	if (( itsSyncHorizontalFlag && itsDirection == kHorizontal) ||
		(!itsSyncHorizontalFlag && itsDirection == kVertical))
	{
		EnforceSpacing();
		CoverChildren();	// contents expanded or contracted
	}
	else if (( itsSyncHorizontalFlag && itsDirection == kVertical) ||
			 (!itsSyncHorizontalFlag && itsDirection == kHorizontal))
	{
		CoverChildren();	// so "full width" is correct
		EnforcePositions();
	}
	else
	{
		assert_msg( 0, "fatal logic error in JXFTCCell::Expand()" );
	}

	return (horizontal ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 BuildChildList (private)

 ******************************************************************************/

void
JXFTCCell::BuildChildList()
{
	JPtrArrayIterator<JXContainer>* iter;
	const bool hasObjs = GetEnclosedObjects(&iter);
	assert( hasObjs );

	assert( itsChildren == nullptr );
	itsChildren = jnew JPtrArray<JXFTCCell>(JPtrArrayT::kForgetAll);

	JXContainer* obj;
	while (iter->Next(&obj))
	{
		auto* cell = dynamic_cast<JXFTCCell*>(obj);
		assert( cell != nullptr );
		itsChildren->AppendItem(cell);
	}

	itsChildren->SetCompareFunction(itsSyncHorizontalFlag ? CompareHorizontally : CompareVertically);
	itsChildren->Sort();

	jdelete iter;
}

/******************************************************************************
 ComputeInvariants (private)

	spacing, elastic item, or positioning

 ******************************************************************************/

void
JXFTCCell::ComputeInvariants()
{
	const JSize cellCount = itsChildren->GetItemCount();
	assert( cellCount > 1 );

	if (( itsSyncHorizontalFlag && itsDirection == kHorizontal) ||
		(!itsSyncHorizontalFlag && itsDirection == kVertical))
	{
		itsChildSpacing = jnew JArray<JCoordinate>;

		JRect prev = itsChildren->GetFirstItem()->GetFrameGlobal();
		for (JIndex i=2; i<=cellCount; i++)
		{
			const JRect r = itsChildren->GetItem(i)->GetFrameGlobal();
			itsChildSpacing->AppendItem(
				itsSyncHorizontalFlag ? r.left - prev.right : r.top - prev.bottom);

			prev = r;
		}
	}

	itsChildPositions = jnew JArray<JFloat>;

	for (auto* cell : *itsChildren)
	{
		const JRect r = cell->GetFrameGlobal();

		const JCoordinate objMin  = (itsSyncHorizontalFlag ? r.left : r.top),
						  thisMin = (itsSyncHorizontalFlag ? itsFrameG.left : itsFrameG.top),
						  objMax  = (itsSyncHorizontalFlag ? r.right : r.bottom),
						  thisMax = (itsSyncHorizontalFlag ? itsFrameG.right : itsFrameG.bottom);
		if (objMin == thisMin && objMax == thisMax)
		{
			itsChildPositions->AppendItem(0);
		}
		else if (objMin == thisMin)
		{
			itsChildPositions->AppendItem(-1);
		}
		else if (objMax == thisMax)
		{
			itsChildPositions->AppendItem(+1);
		}
		else if (itsSyncHorizontalFlag)
		{
			itsChildPositions->AppendItem((r.xcenter() - thisMin) / JFloat(itsFrameG.width()));
		}
		else
		{
			itsChildPositions->AppendItem((r.ycenter() - thisMin) / JFloat(itsFrameG.height()));
		}
	}
}

/******************************************************************************
 EnforceSpacing (private)

 ******************************************************************************/

void
JXFTCCell::EnforceSpacing()
{
	if (theDebugFTCFlag)
	{
		GetFTCLog() << Indent(+1) << "--- Enforcing spacing";
		GetFTCLog() << GetDirectionName() << ":" << std::endl;

		for (JIndex i=1; i<=itsChildSpacing->GetItemCount(); i++)
		{
			GetFTCLog() << Indent(+2) << itsChildSpacing->GetItem(i) << ' ' << itsChildren->GetItem(i)->ToString() << std::endl;
		}
		GetFTCLog() << std::endl;
	}

	const JSize cellCount = itsChildren->GetItemCount();
	assert( cellCount > 1 );

	for (JIndex i=2; i<=cellCount; i++)
	{
		const JRect prev       = itsChildren->GetItem(i-1)->GetFrameGlobal();
		const JRect r          = itsChildren->GetItem(i)->GetFrameGlobal();
		const JCoordinate gap  = (itsSyncHorizontalFlag ? r.left - prev.right : r.top - prev.bottom),
						  orig = itsChildSpacing->GetItem(i-1);
		if (gap == orig)
		{
			continue;
		}
		const JCoordinate delta = orig - gap;

		if (theDebugFTCFlag)
		{
			GetFTCLog() << Indent(+1) << "--- Adjusting gap from " << gap << " -> " << orig << std::endl;
		}

		for (JIndex j=i; j<=cellCount; j++)
		{
			itsChildren->GetItem(j)->Move(
				itsSyncHorizontalFlag ? delta : 0,
				itsSyncHorizontalFlag ? 0 : delta);
		}
	}
}

/******************************************************************************
 EnforcePositions (private)

 ******************************************************************************/

void
JXFTCCell::EnforcePositions()
{
	if (theDebugFTCFlag)
	{
		GetFTCLog() << Indent(+1) << "--- Enforcing positions";
		GetFTCLog() << GetDirectionName() << ":" << std::endl;

		for (JIndex i=1; i<=itsChildPositions->GetItemCount(); i++)
		{
			GetFTCLog() << Indent(+2) << "position: " << itsChildPositions->GetItem(i) << ' ' << itsChildren->GetItem(i)->ToString() << std::endl;
		}
		GetFTCLog() << std::endl;
	}

	const JSize cellCount = itsChildren->GetItemCount();
	assert( cellCount > 1 );

	for (JIndex i=1; i<=cellCount; i++)
	{
		JXFTCCell* cell = itsChildren->GetItem(i);
		const JRect r   = cell->GetFrameGlobal();
		const JFloat p  = itsChildPositions->GetItem(i);

		if (p == 0)						// full width
		{
			cell->AdjustSize(itsSyncHorizontalFlag ? itsFrameG.width() - r.width() : 0,
							 itsSyncHorizontalFlag ? 0 : itsFrameG.height() - r.height());
		}
		else if (p == -1)				// left-aligned
		{
			// nothing to do
		}
		else if (p == +1)				// right-aligned
		{
			cell->Move(itsSyncHorizontalFlag ? itsFrameG.right - r.right : 0,
					   itsSyncHorizontalFlag ? 0 : itsFrameG.bottom - r.bottom);
		}
		else if (itsSyncHorizontalFlag)	// center-line
		{
			const JCoordinate curr  = r.xcenter(),
							  want  = itsFrameG.left + JRound(itsFrameG.width() * p),
							  delta = want - curr;
			if (delta > 0)
			{
				cell->Move(delta, 0);
			}
		}
		else
		{
			const JCoordinate curr  = r.ycenter(),
							  want  = itsFrameG.top + JRound(itsFrameG.height() * p),
							  delta = want - curr;
			if (delta > 0)
			{
				cell->Move(0, delta);
			}
		}
	}
}

/******************************************************************************
 CoverChildren (private)

	resize ourselves to cover contents.

 ******************************************************************************/

void
JXFTCCell::CoverChildren()
{
	const JSize cellCount = itsChildren->GetItemCount();

	JRect r = itsChildren->GetFirstItem()->GetFrameForFTC();
	for (JIndex i=2; i<=cellCount; i++)
	{
		r = JCovering(r, itsChildren->GetItem(i)->GetFrameForFTC());
	}

	itsSyncChildrenFlag = false;
	SetSize(r.width(), r.height());
	itsSyncChildrenFlag = true;
}

/******************************************************************************
 ExpandWidget (private)

	Run fit-to-content on contained objects, if any.  Otherwise, adjust to
	min FTC content size

	Returns the new size.

 ******************************************************************************/

JCoordinate
JXFTCCell::ExpandWidget()
{
	JCoordinate v = 0;
	if (itsWidget->NeedsInternalFTC())
	{
		if (theDebugFTCFlag)
		{
			GetFTCLog() << "=== Processing internal structure for " << itsWidget->ToString() << std::endl;
		}

		const JRect padding = itsWidget->ComputePaddingForInternalFTC();

		if (itsWidget->RunInternalFTC(itsSyncHorizontalFlag, &v))
		{
			v += itsSyncHorizontalFlag ?
					padding.left + padding.right :
					padding.top + padding.bottom;
		}

		if (theDebugFTCFlag)
		{
			GetFTCLog() << "=== Finished processing internal structure for " << itsWidget->ToString() << std::endl;
		}
	}

	if (v == 0)
	{
		v = itsWidget->GetFTCMinContentSize(itsSyncHorizontalFlag);
	}

	const JRect apG         = itsWidget->GetApertureGlobal();
	const JCoordinate delta = v - (itsSyncHorizontalFlag ? apG.width() : apG.height()),
					  dw    = itsSyncHorizontalFlag ? delta : 0,
					  dh    = itsSyncHorizontalFlag ? 0 : delta;
	if (itsWidget->NeedsInternalFTC() && delta != 0)
	{
		if (theDebugFTCFlag)
		{
			GetFTCLog() << "Will adjust widget: dw=" << dw << ", dh=" << dh << std::endl;
		}
		itsWidget->FTCAdjustSize(dw, dh);

		itsSyncChildrenFlag = false;
		AdjustSize(dw, dh);
		itsSyncChildrenFlag = true;
	}
	else if (delta != 0)
	{
		AdjustSize(dw, dh);
	}

	return (itsSyncHorizontalFlag ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXFTCCell::Refresh()
	const
{
	JRect visRectG;
	if (IsVisible() && GetEnclosure()->GetVisibleRectGlobal(itsFrameG, &visRectG))
	{
		GetWindow()->RefreshRect(visRectG);
	}
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXFTCCell::Redraw()
	const
{
	JRect visRectG;
	if (IsVisible() && GetEnclosure()->GetVisibleRectGlobal(itsFrameG, &visRectG))
	{
		GetWindow()->RedrawRect(visRectG);
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXFTCCell::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXFTCCell::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXFTCCell::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.SetDefaultClipRegion(nullptr);
	p.Reset();
	const JSize depth = GetDepth();

	JRect r = frame;
	r.Shrink(depth, depth);

	const JColorID colorList[] =
{
		JColorManager::GetBlueColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetYellowColor(),
		JColorManager::GetLightBlueColor(),
		JColorManager::GetWhiteColor(),
		JColorManager::GetCyanColor()
};
	const JColorID color = colorList[ depth % (sizeof(colorList)/sizeof(JColorID)) ];

	p.SetPenColor(color);
	p.Rect(r);

	p.Line(frame.topLeft(), frame.bottomRight());
	p.Line(frame.topRight(), frame.bottomLeft());
}

/******************************************************************************
 GlobalToLocal (virtual)

 ******************************************************************************/

JPoint
JXFTCCell::GlobalToLocal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x - itsFrameG.left, y - itsFrameG.top);
}

/******************************************************************************
 LocalToGlobal (virtual)

 ******************************************************************************/

JPoint
JXFTCCell::LocalToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x + itsFrameG.left, y + itsFrameG.top);
}

/******************************************************************************
 Place (virtual)

 ******************************************************************************/

void
JXFTCCell::Place
	(
	const JCoordinate enclX,
	const JCoordinate enclY
	)
{
	const JPoint oldPt = GetEnclosure()->GlobalToLocal(itsFrameG.topLeft());
	Move(enclX - oldPt.x, enclY - oldPt.y);
}

/******************************************************************************
 Move (virtual)

 ******************************************************************************/

void
JXFTCCell::Move
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	if (dx != 0 || dy != 0)
	{
		Refresh();		// refresh orig location

		itsFrameG.Shift(dx,dy);

		if (itsSyncChildrenFlag)
		{
			NotifyBoundsMoved(dx,dy);
			SyncWidgetPosition();
		}

		Refresh();		// refresh new location
	}
}

/******************************************************************************
 SyncWidgetPosition (private)

	Synchronize widget to new cell position.

 ******************************************************************************/

void
JXFTCCell::SyncWidgetPosition()
{
	if (itsWidget != nullptr)
	{
		const JRect r        = itsWidget->GetFrameForFTC();
		const JCoordinate dx = itsFrameG.left - r.left,
						  dy = itsFrameG.top - r.top;

		if (theDebugFTCFlag && (dx != 0 || dy != 0))
		{
			GetFTCLog() << Indent() << "Moving widget: " << itsWidget->ToString() << std::endl;
			GetFTCLog() << Indent(+1);
			if (dx != 0) GetFTCLog() << "dx=" << dx;
			if (dy != 0) GetFTCLog() << "dy=" << dy;
			GetFTCLog() << std::endl;
		}

		itsWidget->Move(dx, dy);
	}
}

/******************************************************************************
 SetSize (virtual)

 ******************************************************************************/

void
JXFTCCell::SetSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	AdjustSize(w - itsFrameG.width(), h - itsFrameG.height());
}

/******************************************************************************
 AdjustSize (virtual)

 ******************************************************************************/

void
JXFTCCell::AdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (dw != 0 || dh != 0)
	{
		assert( itsFrameG.width() + dw > 0 && itsFrameG.height() + dh > 0 );

		Refresh();		// refresh orig size

		itsFrameG.bottom += dh;
		itsFrameG.right  += dw;

		if (itsSyncChildrenFlag)
		{
			SyncSize(dw, dh);
		}

		Refresh();		// refresh new size
	}
}

/******************************************************************************
 SyncSize (private)

	Synchronize sizes of contained objects.

 ******************************************************************************/

void
JXFTCCell::SyncSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	if (theDebugFTCFlag)
	{
		GetFTCLog() << Indent() << "Sync " << ToString() << std::endl;
	}

	if (itsWidget != nullptr)
	{
		const JRect r = itsWidget->GetFrameForFTC();

		const JCoordinate fdw = itsFrameG.width() - r.width(),
						  fdh = itsFrameG.height() - r.height();

		if (theDebugFTCFlag && (fdw != 0 || fdh != 0))
		{
			GetFTCLog() << Indent(+1) << "Resizing widget: " << itsWidget->ToString() << std::endl;
			GetFTCLog() << Indent(+2);
			if (fdw != 0) GetFTCLog() << "dw=" << fdw;
			if (fdh != 0) GetFTCLog() << "dh=" << fdh;
			GetFTCLog() << std::endl;
		}

		itsWidget->AdjustSize(fdw, fdh);
		if (itsWidget->NeedsInternalFTC() && !IsElastic())
		{
			if (theDebugFTCFlag)
			{
				GetFTCLog() << "=== Processing internal structure for " << itsWidget->ToString() << std::endl;
			}

			JXFTCCell* root;
			if (itsWidget->FTCBuildLayout(itsSyncHorizontalFlag, &root))
			{
				root->Expand(itsSyncHorizontalFlag);	// NOOP - just sets up state
				root->AdjustSize(fdw, fdh);
				jdelete root;
			}

			if (theDebugFTCFlag)
			{
				GetFTCLog() << "=== Finished processing internal structure for " << itsWidget->ToString() << std::endl;
			}
		}
	}
	else
	{
		// if expanding in the same direction as the cell stack, check for elastic cell

		if ((dw != 0 && itsDirection == kHorizontal) ||
			(dh != 0 && itsDirection == kVertical))
		{
			for (auto* child : *itsChildren)
			{
				if (child->IsElastic())
				{
					if (theDebugFTCFlag)
					{
						GetFTCLog() << Indent(+1) << "Found elastic widget: " << child->ToString() << std::endl;
					}

					child->AdjustSize(dw, dh);
					EnforceSpacing();
					return;
				}
			}
		}

		EnforcePositions();
}
}

/******************************************************************************
 IsElastic (private)

 ******************************************************************************/

bool
JXFTCCell::IsElastic()
{
	if (itsElasticFlag != kUnknown)
	{
		return itsElasticFlag == kTrue;
	}

	itsElasticFlag = kFalse;

	if (itsWidget != nullptr)
	{
		// check for sizing option

		auto* widget = dynamic_cast<JXWidget*>(itsWidget);
		if (widget != nullptr &&
			(( itsSyncHorizontalFlag && widget->GetHSizing() == JXWidget::kHElastic) ||
			 (!itsSyncHorizontalFlag && widget->GetVSizing() == JXWidget::kVElastic)))
		{
			itsElasticFlag = kTrue;
		}
	}
	else if (std::any_of(begin(*itsChildren), end(*itsChildren),
				[] (JXFTCCell* child) { return child->IsElastic(); }))
	{
		itsElasticFlag = kTrue;
	}

	return itsElasticFlag == kTrue;
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Update our DragPainter.

 ******************************************************************************/

void
JXFTCCell::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
}

/******************************************************************************
 EnclosingBoundsMoved (virtual protected)

	Move ourselves so we end up in the same place relative to our
	enclosure.

 ******************************************************************************/

void
JXFTCCell::EnclosingBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	Move(dx,dy);
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXFTCCell::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

 ******************************************************************************/

void
JXFTCCell::EnclosingBoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 GetBoundsGlobal (virtual)

	Returns the bounds in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetBoundsGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 GetFrameGlobal (virtual)

	Returns the frame in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetFrameGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 GetApertureGlobal (virtual)

	Returns the aperture in global coordinates.

 ******************************************************************************/

JRect
JXFTCCell::GetApertureGlobal()
	const
{
	return itsFrameG;
}

/******************************************************************************
 Comparison functions (static private)

 ******************************************************************************/

std::weak_ordering
JXFTCCell::CompareHorizontally
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return c1->itsFrameG.left <=> c2->itsFrameG.left;
}

std::weak_ordering
JXFTCCell::CompareVertically
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return c1->itsFrameG.top <=> c2->itsFrameG.top;
}
