/******************************************************************************
 JX3DWidget.cpp

	BASE CLASS = JXWidget

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JX3DWidget.h>
#include <J3DCamera.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <JMatrix.h>
#include <jMath.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JFloat kSmallZoomScale = 0.01;
const JFloat kZoomScale      = 0.05;
const JFloat kBigZoomScale   = 0.2;

const JFloat kSmallDeltaAngle =  1.0 * kJDegToRad;
const JFloat kDeltaAngle      =  5.0 * kJDegToRad;
const JFloat kBigDeltaAngle   = 30.0 * kJDegToRad;

/******************************************************************************
 Constructor

	Derived classes must call SetCamera().

 ******************************************************************************/

JX3DWidget::JX3DWidget
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsShowFocusFlag(kJTrue),
	itsStartPos(3)
{
	itsCamera = NULL;

	SetBorderWidth(kJXDefaultBorderWidth);
	SetBackColor(GetColormap()->GetBlackColor());
	SetFocusColor(GetColormap()->GetBlackColor());

// can't do this until JXMesaCamera can give the buffer pixmap to Mesa
//	GetWindow()->UsePixmapAsBackground(kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX3DWidget::~JX3DWidget()
{
	delete itsCamera;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JX3DWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsCamera != NULL)
		{
		itsCamera->Render();
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JX3DWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	const JSize borderWidth = GetBorderWidth();
	if (itsShowFocusFlag && HasFocus() && borderWidth > 0)
		{
		p.SetLineWidth(borderWidth);
		p.SetPenColor((p.GetColormap())->GetWhiteColor());
		p.RectInside(frame);
		}
	else
		{
		JXDrawDownFrame(p, frame, borderWidth);
		}
}

/******************************************************************************
 DrawBackground (virtual protected)

	The 3D graphics system draws everything itself.

 ******************************************************************************/

void
JX3DWidget::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Update our view port.

 ******************************************************************************/

void
JX3DWidget::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXWidget::BoundsMoved(dx,dy);

	if (itsCamera != NULL)
		{
		itsCamera->UpdateViewport();
		}
}

/******************************************************************************
 BoundsResized (virtual protected)

	Update our view port.

 ******************************************************************************/

void
JX3DWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidget::BoundsResized(dw,dh);

	if (itsCamera != NULL)
		{
		itsCamera->UpdateViewport();
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JX3DWidget::HandleFocusEvent()
{
	JXWidget::HandleFocusEvent();
	Refresh();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JX3DWidget::HandleUnfocusEvent()
{
	JXWidget::HandleUnfocusEvent();
	Refresh();
}

/******************************************************************************
 BeginDrag (protected)

 ******************************************************************************/

void
JX3DWidget::BeginDrag
	(
	const DragType	type,
	const JPoint&	pt
	)
{
	itsDragType = type;
	itsStartPt  = pt;
	itsStartPos = itsCamera->GetPosition();
}

/******************************************************************************
 ContinueDrag (protected)

	TODO:
		handle up vector != (0,0,1)
		correct scaling of distance along the sphere instead of just using 50

 ******************************************************************************/

void
JX3DWidget::ContinueDrag
	(
	const JPoint& pt
	)
{
	if (itsDragType == kRotateDrag)
		{
		const JVector delta(3, - pt.x + itsStartPt.x, pt.y - itsStartPt.y, 0.0);

		const JVector& o = itsCamera->GetAttentionPt();
		JVector v        = itsStartPos - o;
		const JFloat r   = v.GetLength();
		const JFloat lat = asin(v.GetElement(3) / r);
		const JFloat lon = atan2(v.GetElement(2), v.GetElement(1));

		// direction of drag relative to screen x-axis

		const JFloat alpha = atan2(delta.GetElement(2), delta.GetElement(1));

		// delta along the sphere in drag direction

		const JFloat phi = delta.GetLength() / (r * 50);
		JVector p(3, r*cos(phi), r*sin(phi), 0.0);

		// inverse: rotate around z-axis for partial alignment of x-axis

		JMatrix r1(3, 3);
		r1.SetElements(
			cos(lon), -sin(lon), 0.0,
			sin(lon),  cos(lon), 0.0,
				 0.0,       0.0, 1.0);

		// inverse: rotate around y-axis for complete alignment of x-axis towards viewer

		JMatrix r2(3, 3);
		r2.SetElements(
			cos(lat), 0.0, -sin(lat),
				 0.0, 1.0,       0.0,
			sin(lat), 0.0,  cos(lat));

		// inverse: rotate around x-axis to align y-axis with drag direction

		JMatrix r3(3, 3);
		r3.SetElements(
			1.0,        0.0,         0.0,
			0.0, cos(alpha), -sin(alpha),
			0.0, sin(alpha),  cos(alpha));

		// transform delta long the sphere to our coordinate system

		p = (r1 * r2 * r3 * p).GetColVector(1);

		itsCamera->SetPosition(p + o);
		}
}

/******************************************************************************
 FinishRotationDrag (protected)

 ******************************************************************************/

void
JX3DWidget::FinishDrag()
{
	itsDragType = kInvalidDrag;
}

/******************************************************************************
 ZoomForWheel (protected)

	Call this to support zooming via a wheel mouse.  Returns kJTrue if
	it handled the event.

	Ctrl:  larger step size
	Shift: smaller step size

 ******************************************************************************/

JBoolean
JX3DWidget::ZoomForWheel
	(
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	JCoordinate delta;
	if (button == kJXButton4)
		{
		delta = -1;
		}
	else if (button == kJXButton5)
		{
		delta = +1;
		}
	else
		{
		return kJFalse;
		}

	const JVector& o = itsCamera->GetAttentionPt();
	const JVector& p = itsCamera->GetPosition();

	JFloat scale =
		(modifiers.shift()   ? kSmallZoomScale :
		(modifiers.control() ? kBigZoomScale   :
		kZoomScale));

#ifdef _J_OSX
	scale /= 10.0;
#endif

	itsCamera->SetPosition(o + (p - o) * (1.0 + delta * scale));
	return kJTrue;
}

/******************************************************************************
 HandleRotationKeyPress (virtual)

	Call this to support rotating/zooming via the arrow keys.  Returns
	kJTrue if it handled the event.

	Meta-up/down: -/+ radius
	up/down     : +/- latitude
	left/right  : -/+ longitude

	Ctrl : larger step size
	Shift: smaller step size

 ******************************************************************************/

JBoolean
JX3DWidget::HandleRotationKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (modifiers.meta() && key == kJUpArrow)
		{
		return ZoomForWheel(kJXButton4, modifiers);
		}
	else if (modifiers.meta() && key == kJDownArrow)
		{
		return ZoomForWheel(kJXButton5, modifiers);
		}

	const JVector& o = itsCamera->GetAttentionPt();
	JVector v        = itsCamera->GetPosition() - o;
	const JFloat r   = v.GetLength();
	JFloat lat       = asin(v.GetElement(3) / r);
	JFloat lon       = atan2(v.GetElement(2), v.GetElement(1));

	const JFloat delta =
		(modifiers.shift()   ? kSmallDeltaAngle :
		(modifiers.control() ? kBigDeltaAngle   :
		kDeltaAngle));

	JBoolean moved = kJFalse;

	if (key == kJUpArrow)
		{
		lat  += delta;
		moved = kJTrue;
		}
	else if (key == kJDownArrow)
		{
		lat  -= delta;
		moved = kJTrue;
		}

	else if (key == kJLeftArrow)
		{
		lon  += delta;
		moved = kJTrue;
		}
	else if (key == kJRightArrow)
		{
		lon  -= delta;
		moved = kJTrue;
		}

	if (moved)
		{
		itsCamera->SetPosition(JVector(3,
			r * cos(lat) * cos(lon),
			r * cos(lat) * sin(lon),
			r * sin(lat)));

		Redraw();	// key repeats can delay Refresh()
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
