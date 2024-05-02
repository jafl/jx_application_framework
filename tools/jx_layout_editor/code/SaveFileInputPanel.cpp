/******************************************************************************
 SaveFileInputPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "SaveFileInputPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SaveFileInputPanel::SaveFileInputPanel
	(
	WidgetParametersDialog* dlog,

	const bool spaces
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);

	itsAllowSpacesCB =
		jnew JXTextCheckbox(JGetString("itsAllowSpacesCB::SaveFileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 180,20);
	itsAllowSpacesCB->SetShortcuts(JGetString("itsAllowSpacesCB::shortcuts::SaveFileInputPanel::Panel"));

// end Panel

	dlog->AddPanel(this, container);

	itsAllowSpacesCB->SetState(spaces);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SaveFileInputPanel::~SaveFileInputPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
SaveFileInputPanel::GetValues
	(
	bool* spaces
	)
{
	*spaces = itsAllowSpacesCB->IsChecked();
}
