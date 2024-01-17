/******************************************************************************
 PathInputPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "PathInputPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PathInputPanel::PathInputPanel
	(
	WidgetParametersDialog* dlog,

	const bool required,
	const bool invalid,
	const bool write
	)
{
	BuildPanel(dlog, required, invalid, write);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PathInputPanel::~PathInputPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
PathInputPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	const bool required,
	const bool invalid,
	const bool write
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	itsPathRequiredCB =
		jnew JXTextCheckbox(JGetString("itsPathRequiredCB::PathInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 80,20);
	itsPathRequiredCB->SetShortcuts(JGetString("itsPathRequiredCB::shortcuts::PathInputPanel::Panel"));

	itsAllowInvalidPathCB =
		jnew JXTextCheckbox(JGetString("itsAllowInvalidPathCB::PathInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 130,20);
	itsAllowInvalidPathCB->SetShortcuts(JGetString("itsAllowInvalidPathCB::shortcuts::PathInputPanel::Panel"));

	itsRequirePathWritableCB =
		jnew JXTextCheckbox(JGetString("itsRequirePathWritableCB::PathInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,10, 60,20);
	itsRequirePathWritableCB->SetShortcuts(JGetString("itsRequirePathWritableCB::shortcuts::PathInputPanel::Panel"));

// end Panel

	dlog->AddPanel(this, container);

	itsPathRequiredCB->SetState(required);
	itsAllowInvalidPathCB->SetState(invalid);
	itsRequirePathWritableCB->SetState(write);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
PathInputPanel::GetValues
	(
	bool* required,
	bool* invalid,
	bool* write
	)
{
	*required = itsPathRequiredCB->IsChecked();
	*invalid  = itsAllowInvalidPathCB->IsChecked();
	*write    = itsRequirePathWritableCB->IsChecked();
}
