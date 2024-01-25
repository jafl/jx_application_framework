/******************************************************************************
 TestSliderDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestSliderDirector.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXSlider.h>
#include <jx-af/jx/JXLevelControl.h>
#include <jx-af/jx/jXGlobals.h>
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

	auto* window = jnew JXWindow(this, 230,240, JGetString("WindowTitle::TestSliderDirector::JXLayout"));
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestSliderDirector");

	itsMaxSlider =
		jnew JXSlider(window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 60,20, 30,110);

	itsMaxLevel =
		jnew JXLevelControl(window,
					JXWidget::kFixedRight, JXWidget::kVElastic, 150,20, 20,110);

	itsSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,150, 190,30);

	itsLevel =
		jnew JXLevelControl(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,200, 190,20);

// end JXLayout

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
