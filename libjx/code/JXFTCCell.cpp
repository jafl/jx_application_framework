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
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFTCCell::JXFTCCell
	(
	JXContainer* matchObj,
	JXContainer* enc
	)
	:
	JXContainer(enc->GetWindow(), enc),
	itsWidget(matchObj)
{
	if (itsWidget != NULL)
		{
		itsFrameG = itsWidget->GetFrameForExpandToFitContent();
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
	s << " (" << GetDepth() << ")";
	if (itsWidget != NULL)
		{
		s << " (" << itsWidget->ToString() << ")";
		}
	return  JString(s.str());
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
	const JCoordinate dwb,
	const JCoordinate dhb
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
