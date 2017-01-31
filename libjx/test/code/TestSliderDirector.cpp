/******************************************************************************
 TestSliderDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestSliderDirector.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXSlider.h>
#include <JXLevelControl.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestSliderDirector::TestSliderDirector
	(
	JXWindowDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestSliderDirector::~TestSliderDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestSliderDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 230,240, JString::empty);
	assert( window != NULL );

	itsMaxSlider =
		jnew JXSlider(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 60,20, 30,110);
	assert( itsMaxSlider != NULL );

	itsMaxLevel =
		jnew JXLevelControl(window,
					JXWidget::kFixedRight, JXWidget::kVElastic, 150,20, 20,110);
	assert( itsMaxLevel != NULL );

	itsSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,150, 190,30);
	assert( itsSlider != NULL );

	itsLevel =
		jnew JXLevelControl(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,200, 190,20);
	assert( itsLevel != NULL );

// end JXLayout

	window->SetTitle("Testing Sliders");
	window->SetWMClass("testjx", "TestSliderDirector");

	itsSlider->SetRange(1,10);
	itsSlider->SetHint("Horizontal slider");

	itsMaxSlider->SetRange(1,10);
	itsMaxSlider->SetValue(itsSlider->GetMaxValue());
	itsMaxSlider->SetHint("Vertical slider (controls maximum value of horizontal slider)");
	ListenTo(itsMaxSlider);

	itsLevel->SetRange(1,10);
	itsLevel->SetHint("Horizontal level control");

	itsMaxLevel->SetRange(1,10);
	itsMaxLevel->SetValue(itsLevel->GetMaxValue());
	itsMaxLevel->SetHint("Vertical level control (controls maximum value of horizontal level control)");
	ListenTo(itsMaxLevel);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TestSliderDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMaxSlider && message.Is(JSliderBase::kMoved))
		{
		itsSlider->SetMaxValue(itsMaxSlider->GetValue());
		}

	else if (sender == itsMaxLevel && message.Is(JSliderBase::kMoved))
		{
		itsLevel->SetMaxValue(itsMaxLevel->GetValue());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}
