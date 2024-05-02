/******************************************************************************
 FloatInputPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "FloatInputPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FloatInputPanel::FloatInputPanel
	(
	WidgetParametersDialog* dlog,

	const bool		required,
	const bool		hasMin,
	const JFloat	min,
	const bool		hasMax,
	const JFloat	max
	)
{
	auto* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);

	itsFloatRequiredCB =
		jnew JXTextCheckbox(JGetString("itsFloatRequiredCB::FloatInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	itsFloatRequiredCB->SetShortcuts(JGetString("itsFloatRequiredCB::shortcuts::FloatInputPanel::Panel"));

	auto* floatMinimumValueLabel =
		jnew JXStaticText(JGetString("floatMinimumValueLabel::FloatInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 70,20);
	floatMinimumValueLabel->SetToLabel(false);

	auto* floatMaximumValueLabel =
		jnew JXStaticText(JGetString("floatMaximumValueLabel::FloatInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,10, 80,20);
	floatMaximumValueLabel->SetToLabel(false);

	itsFloatMinValueInput =
		jnew JXFloatInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,10, 80,20);
	itsFloatMinValueInput->SetIsRequired(false);

	itsFloatMaxValueInput =
		jnew JXFloatInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,10, 80,20);
	itsFloatMaxValueInput->SetIsRequired(false);

// end Panel

	dlog->AddPanel(this, container);

	itsFloatRequiredCB->SetState(required);

	if (hasMin)
	{
		itsFloatMinValueInput->SetValue(min);
	}
	else
	{
		itsFloatMinValueInput->GetText()->SetText(JString::empty);
	}

	if (hasMax)
	{
		itsFloatMaxValueInput->SetValue(max);
	}
	else
	{
		itsFloatMaxValueInput->GetText()->SetText(JString::empty);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FloatInputPanel::~FloatInputPanel()
{
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
FloatInputPanel::Validate()
	const
{
	JFloat min, max;
	if (itsFloatMinValueInput->GetValue(&min) &&
		itsFloatMaxValueInput->GetValue(&max) &&
		max < min)
	{
		JGetUserNotification()->ReportError(
			JGetString("MinLessThanMax::ValueInputPanel"));
		JXFocusWidgetTask::Focus(itsFloatMinValueInput);
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
FloatInputPanel::GetValues
	(
	bool*	required,
	bool*	hasMin,
	JFloat*	min,
	bool*	hasMax,
	JFloat*	max
	)
{
	*required = itsFloatRequiredCB->IsChecked();
	*hasMin   = itsFloatMinValueInput->GetValue(min);
	*hasMax   = itsFloatMaxValueInput->GetValue(max);
}
