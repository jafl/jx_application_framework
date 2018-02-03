/******************************************************************************
 THX3DPlotWidget.cpp

	BASE CLASS = JX3DWidget

	Ideally, we should create J3DSurface

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include "THX3DPlotWidget.h"
#include <JXMesaCamera.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <J3DUniverse.h>
#include <J3DAxes.h>
#include <J3DSurface.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

THX3DPlotWidget::THX3DPlotWidget
	(
	J3DUniverse*		universe,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JX3DWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsMesaCamera = jnew JXMesaCamera(this, universe);
	assert( itsMesaCamera != NULL );
	SetCamera(itsMesaCamera);

	// create the axes

	itsAxes = jnew J3DAxes(*(GetColormap()), universe, 5.0);
	assert( itsAxes != NULL );

	// create an empty surface

	itsSurface = jnew J3DSurface(universe);
	assert( itsSurface != NULL );

	itsSurface->SetColors(
		JRGB(kJMaxRGBValue, 0, 0),				// red
		JRGB(kJMaxRGBValue, kJMaxRGBValue, 0),	// yellow
		JRGB(0, kJMaxRGBValue, 0));				// green

	// place the camera

	const JFloat r   = 12.0;
	const JFloat lat = 45.0 * kJDegToRad;
	const JFloat lon = 45.0 * kJDegToRad;

	GetCamera()->SetPosition(JVector(3,
		r * cos(lat) * cos(lon),
		r * cos(lat) * sin(lon),
		r * sin(lat)));

	AdjustFarZ();

	// handle the arrow keys

	WantInput(kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX3DPlotWidget::~THX3DPlotWidget()
{
	jdelete itsAxes;
	jdelete itsSurface;
}

/******************************************************************************
 PlotData

	The data is passed in a single JMatrix because it is too inefficient to
	store a 2D array of JVectors, especially since we may get a *lot* of
	points.

	The data must be a staggered grid grouped by x value.  The columns of
	points must contain yCount, yCount-1, yCount, ..., yCount-1, yCount
	values, respectively.

	*** itsSurface takes ownership of the data.

 ******************************************************************************/

void
THX3DPlotWidget::PlotData
	(
	const JSize	xCount,
	const JSize	yCount,
	JMatrix*	data
	)
{
	itsSurface->SetData(xCount, yCount, data);
}

/******************************************************************************
 HandleKeyPress (virtual)

	Meta-up/down: -/+ radius
	up/down     : +/- latitude
	left/right  : -/+ longitude

	Shift: larger step size

 ******************************************************************************/

void
THX3DPlotWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (HandleRotationKeyPress(key, modifiers))
		{
		// work has been done
		AdjustFarZ();
		}
	else
		{
		JX3DWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
THX3DPlotWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button > kJXRightButton)
		{
		ZoomForWheel(button, modifiers);
		AdjustFarZ();
		}
	else
		{
		BeginDrag(kRotateDrag, pt);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
THX3DPlotWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ContinueDrag(pt);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
THX3DPlotWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishDrag();
}


/******************************************************************************
 AdjustFarZ (private)

 ******************************************************************************/

void
THX3DPlotWidget::AdjustFarZ()
{
	const JVector r = itsMesaCamera->GetPosition() - itsMesaCamera->GetAttentionPt();
	itsMesaCamera->SetFarZ(2*r.GetLength());
}
