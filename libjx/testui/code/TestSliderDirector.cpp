/******************************************************************************
 TestSliderDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestSliderDirector.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXSlider.h>
#include <jx-af/jx/JXLevelControl.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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

	auto* window = jnew JXWindow(this, 230,240, JString::empty);
	assert( window != nullptr );

	itsMaxSlider =
		jnew JXSlider(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 60,20, 30,110);
	assert( itsMaxSlider != nullptr );

	itsMaxLevel =
		jnew JXLevelControl(window,
					JXWidget::kFixedRight, JXWidget::kVElastic, 150,20, 20,110);
	assert( itsMaxLevel != nullptr );

	itsSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,150, 190,30);
	assert( itsSlider != nullptr );

	itsLevel =
		jnew JXLevelControl(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,200, 190,20);
	assert( itsLevel != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestSliderDirector"));
	window->SetWMClass("testjx", "TestSliderDirector");

	itsSlider->SetRange(1,10);
	itsSlider->SetHint(JGetString("HorizontalSliderHint::TestSliderDirector"));

	itsMaxSlider->SetRange(1,10);
	itsMaxSlider->SetValue(itsSlider->GetMaxValue());
	itsMaxSlider->SetHint(JGetString("VerticalSliderHint::TestSliderDirector"));
	ListenTo(itsMaxSlider, std::function([this](const JSliderBase::Moved& msg)
	{
		itsSlider->SetMaxValue(msg.GetValue());
	}));

	itsLevel->SetRange(1,10);
	itsLevel->SetHint(JGetString("HorizontalLevelControlHint::TestSliderDirector"));

	itsMaxLevel->SetRange(1,10);
	itsMaxLevel->SetValue(itsLevel->GetMaxValue());
	itsMaxLevel->SetHint(JGetString("VerticalLevelControlHint::TestSliderDirector"));
	ListenTo(itsMaxLevel, std::function([this](const JSliderBase::Moved& msg)
	{
		itsLevel->SetMaxValue(msg.GetValue());
	}));
}
