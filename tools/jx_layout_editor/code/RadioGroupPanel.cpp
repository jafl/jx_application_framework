/******************************************************************************
 RadioGroupPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "RadioGroupPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RadioGroupPanel::RadioGroupPanel
	(
	WidgetParametersDialog* dlog,

	const bool hideBorder
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);

	itsHideBorderCB =
		jnew JXTextCheckbox(JGetString("itsHideBorderCB::RadioGroupPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 100,20);
	itsHideBorderCB->SetShortcuts(JGetString("itsHideBorderCB::shortcuts::RadioGroupPanel::Panel"));

// end Panel

	dlog->AddPanel(this, container);

	itsHideBorderCB->SetState(hideBorder);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RadioGroupPanel::~RadioGroupPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
RadioGroupPanel::GetValues
	(
	bool* hideBorder
	)
{
	*hideBorder = itsHideBorderCB->IsChecked();
}
