/******************************************************************************
 IntegerInputPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "IntegerInputPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

IntegerInputPanel::IntegerInputPanel
	(
	WidgetParametersDialog* dlog,

	const bool		required,
	const bool		hasMin,
	const JInteger	min,
	const bool		hasMax,
	const JInteger	max
	)
{
	BuildPanel(dlog, required, hasMin, min, hasMax, max);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

IntegerInputPanel::~IntegerInputPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
IntegerInputPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	const bool		required,
	const bool		hasMin,
	const JInteger	min,
	const bool		hasMax,
	const JInteger	max
	)
{
	auto* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	itsIntRequiredCB =
		jnew JXTextCheckbox(JGetString("itsIntRequiredCB::IntegerInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	assert( itsIntRequiredCB != nullptr );
	itsIntRequiredCB->SetShortcuts(JGetString("itsIntRequiredCB::IntegerInputPanel::shortcuts::Panel"));

	itsIntMinValueInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,10, 80,20);
	assert( itsIntMinValueInput != nullptr );

	auto* integerMinimumValueLabel =
		jnew JXStaticText(JGetString("integerMinimumValueLabel::IntegerInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 70,20);
	assert( integerMinimumValueLabel != nullptr );
	integerMinimumValueLabel->SetToLabel();

	itsIntMaxValueInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,10, 80,20);
	assert( itsIntMaxValueInput != nullptr );

	auto* integerMaximumValueLabel =
		jnew JXStaticText(JGetString("integerMaximumValueLabel::IntegerInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,10, 80,20);
	assert( integerMaximumValueLabel != nullptr );
	integerMaximumValueLabel->SetToLabel();

// end Panel

	dlog->AddPanel(this, container);

	itsIntRequiredCB->SetState(required);

	itsIntMinValueInput->SetIsRequired(false);
	if (hasMin)
	{
		itsIntMinValueInput->SetValue(min);
	}
	else
	{
		itsIntMinValueInput->GetText()->SetText(JString::empty);
	}

	itsIntMaxValueInput->SetIsRequired(false);
	if (hasMax)
	{
		itsIntMaxValueInput->SetValue(max);
	}
	else
	{
		itsIntMaxValueInput->GetText()->SetText(JString::empty);
	}
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
IntegerInputPanel::Validate()
	const
{
	JInteger min, max;
	if (itsIntMinValueInput->GetValue(&min) &&
		itsIntMaxValueInput->GetValue(&max) &&
		max < min)
	{
		JGetUserNotification()->ReportError(
			JGetString("MinLessThanMax::ValueInputPanel"));
		JXFocusWidgetTask::Focus(itsIntMinValueInput);
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
IntegerInputPanel::GetValues
	(
	bool*		required,
	bool*		hasMin,
	JInteger*	min,
	bool*		hasMax,
	JInteger*	max
	)
{
	*required = itsIntRequiredCB->IsChecked();
	*hasMin   = itsIntMinValueInput->GetValue(min);
	*hasMax   = itsIntMaxValueInput->GetValue(max);
}
