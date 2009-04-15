/******************************************************************************
 Test3DWidget.cpp

	BASE CLASS = JX3DWidget

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "Test3DWidget.h"
#include <JXMesaCamera.h>
#include <jXGlobals.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Test3DWidget::Test3DWidget
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
	JXMesaCamera* camera = new JXMesaCamera(this, universe);
	assert( camera != NULL );
	SetCamera(camera);

	// place the camera

	const JFloat r   = 3.0;
	const JFloat lat = 45.0 * kJDegToRad;
	const JFloat lon = 45.0 * kJDegToRad;

	(GetCamera())->SetPosition(JVector(3,
		r * cos(lat) * cos(lon),
		r * cos(lat) * sin(lon),
		r * sin(lat)));

	// handle the arrow keys

	WantInput(kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Test3DWidget::~Test3DWidget()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

	Meta-up/down: -/+ radius
	up/down     : +/- latitude
	left/right  : -/+ longitude

	Shift: larger step size

 ******************************************************************************/

void
Test3DWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (HandleRotationKeyPress(key, modifiers))
		{
		// work has been done
		}

	else if (key == 'q')
		{
		(JXGetApplication())->Quit();
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
Test3DWidget::HandleMouseDown
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
Test3DWidget::HandleMouseDrag
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
Test3DWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	FinishDrag();
}
