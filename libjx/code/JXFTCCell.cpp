/******************************************************************************
 JXFTCCell.cpp

	Represents a cell in the nested tables constructed by
	JXContainer::ExpandToFitContent.

	BASE CLASS = JXContainer

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXFTCCell.h>
#include <JXWindow.h>
#include <JXDisplay.h>
#include <JXDragPainter.h>
#include <JXDNDManager.h>
#include <JXColormap.h>
#include <JOrderedSetUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFTCCell::JXFTCCell
	(
	JXContainer*	matchObj,
	JXContainer*	enc,
	const Direction	direction
	)
	:
	JXContainer(enc->GetWindow(), enc),
	itsWidget(matchObj),
	itsDirection(direction),
	itsSyncChildrenFlag(kJFalse)
{
	if (itsWidget != NULL)
		{
		itsFrameG = itsWidget->GetFrameForFTC();
		}

	Refresh();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFTCCell::~JXFTCCell()
{
	JXWindow* window = GetWindow();
	if (IsVisible())
		{
		window->RefreshRect(itsFrameG);
		}
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
	s << (itsDirection == kHorizontal ? " (horiz)" : itsDirection == kVertical ? " (vert)" : "");
	if (itsWidget != NULL)
		{
		s << " (" << itsWidget->ToString() << ")";
		}
	return JString(s.str());
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
	while (p != NULL)
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
	const JBoolean horizontal
	)
{
	itsSyncChildrenFlag = kJTrue;

	if (itsWidget != NULL)
		{
		const JCoordinate size = ExpandWidget(horizontal);
		const JRect r = itsWidget->GetFrameForFTC();
		SetSize(r.width(), r.height());
		return size;
		}

	assert( itsDirection != kNoDirection );

	if (theDebugFTCFlag && GetEnclosure() == GetWindow())
		{
		std::cout << "----------" << std::endl;
		std::cout << "ExpandLayout (" << (horizontal ? "horiz" : "vert") << "):" << std::endl;
		}
	else if (theDebugFTCFlag)
		{
		std::cout << Indent() << "Checking cell " << ToString() << std::endl;
		}

	// get list of contained JXFTCCell's

	JPtrArrayIterator<JXContainer>* iter;
	const JBoolean hasObjs = GetEnclosedObjects(&iter);
	assert( hasObjs );

	JPtrArray<JXFTCCell> cellList(JPtrArrayT::kForgetAll);
	JXContainer* obj;
	while (iter->Next(&obj))
		{
		JXFTCCell* cell = dynamic_cast<JXFTCCell*>(obj);
		assert( cell != NULL );
		cellList.AppendElement(cell);
		}

	jdelete iter;
	iter = NULL;

	cellList.SetCompareFunction(horizontal ? CompareHorizontally : CompareVertically);
	cellList.Sort();

	// compute invariants: spacing or positioning

	const JSize cellCount = cellList.GetElementCount();
	assert( cellCount > 1 );

	JArray<JCoordinate> spacing;
	JArray<JFloat> position;	// -1: left, +1:right, 0:both, else:center-line
	if (( horizontal && itsDirection == kHorizontal) ||
		(!horizontal && itsDirection == kVertical))
		{
		JRect prev = cellList.GetFirstElement()->GetFrameGlobal();
		for (JIndex i=2; i<=cellCount; i++)
			{
			const JRect r = cellList.GetElement(i)->GetFrameGlobal();
			spacing.AppendElement(horizontal ? r.left - prev.right : r.top - prev.bottom);
			prev = r;
			}
		}
	else if (( horizontal && itsDirection == kVertical) ||
			 (!horizontal && itsDirection == kHorizontal))
		{
		for (JIndex i=1; i<=cellCount; i++)
			{
			const JRect r = cellList.GetElement(i)->GetFrameGlobal();

			const JCoordinate objMin  = (horizontal ? r.left : r.top),
							  thisMin = (horizontal ? itsFrameG.left : itsFrameG.top),
							  objMax  = (horizontal ? r.right : r.bottom),
							  thisMax = (horizontal ? itsFrameG.right : itsFrameG.bottom);
			if (objMin == thisMin && objMax == thisMax)
				{
				position.AppendElement(0);
				}
			else if (objMin == thisMin)
				{
				position.AppendElement(-1);
				}
			else if (objMax == thisMax)
				{
				position.AppendElement(+1);
				}
			else if (horizontal)
				{
				position.AppendElement((r.xcenter() - thisMin) / JFloat(itsFrameG.width()));
				}
			else
				{
				position.AppendElement((r.ycenter() - thisMin) / JFloat(itsFrameG.height()));
				}
			}
		}
	else
		{
		assert_msg( 0, "fatal logic error in JXFTCCell" );
		}

	// let each widget expand

	JCoordinate max = 0;
	for (JIndex i=1; i<=cellCount; i++)
		{
		max = JMax(max, cellList.GetElement(i)->Expand(horizontal));
		}

	// enforce invariants

	if (theDebugFTCFlag)
		{
		std::cout << Indent(+1) << "--- Enforcing invariants: ";

		if (!spacing.IsEmpty())
			{
			std::cout << "spacing:";
			for (JIndex i=1; i<=spacing.GetElementCount(); i++)
				{
				std::cout << ' ' << spacing.GetElement(i);
				}
			}
		else if (!position.IsEmpty())
			{
			std::cout << "position:";
			for (JIndex i=1; i<=position.GetElementCount(); i++)
				{
				std::cout << ' ' << position.GetElement(i);
				}
			}
		std::cout << std::endl;
		}

	if (( horizontal && itsDirection == kHorizontal) ||
		(!horizontal && itsDirection == kVertical))
		{
		for (JIndex i=2; i<=cellCount; i++)
			{
			const JRect prev       = cellList.GetElement(i-1)->GetFrameGlobal();
			const JRect r          = cellList.GetElement(i)->GetFrameGlobal();
			const JCoordinate gap  = (horizontal ? r.left - prev.right : r.top - prev.bottom),
							  orig = spacing.GetElement(i-1);
			if (gap == orig)
				{
				continue;
				}
			const JCoordinate delta = orig - gap;

			if (theDebugFTCFlag)
				{
				std::cout << Indent() << "--- Adjusting gap from " << gap << " -> " << orig << std::endl;
				}

			for (JIndex j=i; j<=cellCount; j++)
				{
				JXFTCCell* cell = cellList.GetElement(j);
				cell->Move(horizontal ? delta : 0, horizontal ? 0 : delta);
				}
			}

		CoverChildren(cellList);
		}
	else if (( horizontal && itsDirection == kVertical) ||
			 (!horizontal && itsDirection == kHorizontal))
		{
		CoverChildren(cellList);

		for (JIndex i=1; i<=cellCount; i++)
			{
			JXFTCCell* cell = cellList.GetElement(i);
			const JRect r   = cell->GetFrameGlobal();
			const JFloat p  = position.GetElement(i);

			if (p == 0)				// full width
				{
				cell->AdjustSize(horizontal ? itsFrameG.width() - r.width() : 0,
								 horizontal ? 0 : itsFrameG.height() - r.height());
				}
			else if (p == -1)		// left-aligned
				{
				// nothing to do
				}
			else if (p == +1)		// right-aligned
				{
				cell->Move(horizontal ? itsFrameG.right - r.right : 0,
						   horizontal ? 0 : itsFrameG.bottom - r.bottom);
				}
			else if (horizontal)	// center-line
				{
				const JCoordinate curr = r.xcenter(),
								  want = itsFrameG.left + JRound(itsFrameG.width() * p);
				cell->Move(want - curr, 0);
				}
			else
				{
				const JCoordinate curr = r.ycenter(),
								  want = itsFrameG.top + JRound(itsFrameG.height() * p);
				cell->Move(0, want - curr);
				}
			}
		}
	else
		{
		assert_msg( 0, "fatal logic error in JXFTCCell" );
		}

	return (horizontal ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 CoverChildren (private)

	resize ourselves to cover contents.

 ******************************************************************************/

void
JXFTCCell::CoverChildren
	(
	const JPtrArray<JXFTCCell>& cellList
	)
{
	const JSize cellCount = cellList.GetElementCount();

	JRect r = cellList.GetFirstElement()->GetFrameForFTC();
	for (JIndex i=2; i<=cellCount; i++)
		{
		r = JCovering(r, cellList.GetElement(i)->GetFrameForFTC());
		}

	itsSyncChildrenFlag = kJFalse;
	SetSize(r.width(), r.height());
	itsSyncChildrenFlag = kJTrue;
}

/******************************************************************************
 ExpandWidget (private)

	Run fit-to-content on contained objects, if any.  Otherwise, adjust to
	min FTC content size

	Returns the new size.

 ******************************************************************************/

JCoordinate
JXFTCCell::ExpandWidget
	(
	const JBoolean horizontal
	)
{
	JCoordinate v = 0;
	if (itsWidget->NeedsInternalFTC())
		{
		if (theDebugFTCFlag)
			{
			std::cout << "=== Processing internal structure for" << itsWidget->ToString() << std::endl;
			}

		JXFTCCell* root = itsWidget->FTCBuildLayout(horizontal);
		if (root != NULL)
			{
			v = root->Expand(horizontal);
			jdelete root;

			if (theDebugFTCFlag)
				{
				std::cout << "=== Finished processing internal structure" << std::endl;
				}
			}
		}

	if (v == 0)
		{
		v = itsWidget->GetFTCMinContentSize(horizontal);
		}

	const JRect apG         = itsWidget->GetApertureGlobal();
	const JCoordinate delta = v - (horizontal ? apG.width() : apG.height());
	if (delta != 0)
		{
		AdjustSize(horizontal ? delta : 0, horizontal ? 0 : delta);
		}

	return (horizontal ? itsFrameG.width() : itsFrameG.height());
}

/******************************************************************************
 SyncWidgetPosition (private)

	Synchronize widgets to new cell positions.

 ******************************************************************************/

void
JXFTCCell::SyncWidgetPosition()
{
	if (itsWidget == NULL)
		{
		return;
		}

	const JRect r        = itsWidget->GetFrameForFTC();
	const JCoordinate dx = itsFrameG.left - r.left,
					  dy = itsFrameG.top - r.top;

	if (theDebugFTCFlag && (dx != 0 || dy != 0))
		{
		std::cout << Indent() << "Moving widget: " << itsWidget->ToString() << std::endl;
		std::cout << Indent(+1);
		if (dx != 0) std::cout << "dx=" << dx;
		if (dy != 0) std::cout << "dy=" << dy;
		std::cout << std::endl;
		}

	itsWidget->Move(dx, dy);
}

/******************************************************************************
 SyncWidgetSize (private)

	Synchronize sizes of contained objects.

 ******************************************************************************/

void
JXFTCCell::SyncWidgetSize()
{
	if (itsWidget == NULL)
		{
		return;
		}

	const JRect r = itsWidget->GetFrameForFTC();
	const JCoordinate dw = itsFrameG.width() - r.width(),
					  dh = itsFrameG.height() - r.height();

	if (theDebugFTCFlag && (dw != 0 || dh != 0))
		{
		std::cout << Indent() << "Resizing widget: " << itsWidget->ToString() << std::endl;
		std::cout << Indent(+1);
		if (dw != 0) std::cout << "dw=" << dw;
		if (dh != 0) std::cout << "dh=" << dh;
		std::cout << std::endl;
		}

	itsWidget->AdjustSize(dw, dh);
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
	p.SetDefaultClipRegion(NULL);
	p.Reset();
	const JSize depth = GetDepth();

	JRect r = frame;
	r.Shrink(depth, depth);

	const JColorIndex colorList[] =
	{
		p.GetColormap()->GetBlueColor(),
		p.GetColormap()->GetRedColor(),
		p.GetColormap()->GetYellowColor(),
		p.GetColormap()->GetLightBlueColor(),
		p.GetColormap()->GetWhiteColor(),
		p.GetColormap()->GetCyanColor()
	};
	const JColorIndex color = colorList[ depth % (sizeof(colorList)/sizeof(JColorIndex)) ];

	p.SetPenColor(color);
	p.JPainter::Rect(r);

	p.JPainter::Line(frame.topLeft(), frame.bottomRight());
	p.JPainter::Line(frame.topRight(), frame.bottomLeft());
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
			// TODO:  check for sizing options

			NotifyBoundsResized(dw,dh);
			SyncWidgetSize();
			}

		Refresh();		// refresh new size
		}
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
	AdjustSize(dw,dh);
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

JOrderedSetT::CompareResult
JXFTCCell::CompareHorizontally
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return JCompareCoordinates(
		c1->itsFrameG.left,
		c2->itsFrameG.left);
}

JOrderedSetT::CompareResult
JXFTCCell::CompareVertically
	(
	JXFTCCell* const & c1,
	JXFTCCell* const & c2
	)
{
	return JCompareCoordinates(
		c1->itsFrameG.top,
		c2->itsFrameG.top);
}
