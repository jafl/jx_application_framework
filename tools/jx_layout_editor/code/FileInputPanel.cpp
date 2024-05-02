/******************************************************************************
 FileInputPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "FileInputPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileInputPanel::FileInputPanel
	(
	WidgetParametersDialog* dlog,

	const bool required,
	const bool invalid,
	const bool read,
	const bool write,
	const bool exec
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);

	itsFileRequiredCB =
		jnew JXTextCheckbox(JGetString("itsFileRequiredCB::FileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 80,20);
	itsFileRequiredCB->SetShortcuts(JGetString("itsFileRequiredCB::shortcuts::FileInputPanel::Panel"));

	itsAllowInvalidFileCB =
		jnew JXTextCheckbox(JGetString("itsAllowInvalidFileCB::FileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 120,20);
	itsAllowInvalidFileCB->SetShortcuts(JGetString("itsAllowInvalidFileCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileReadCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileReadCB::FileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,10, 60,20);
	itsRequireFileReadCB->SetShortcuts(JGetString("itsRequireFileReadCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileWriteCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileWriteCB::FileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,10, 60,20);
	itsRequireFileWriteCB->SetShortcuts(JGetString("itsRequireFileWriteCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileExecCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileExecCB::FileInputPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 380,10, 60,20);
	itsRequireFileExecCB->SetShortcuts(JGetString("itsRequireFileExecCB::shortcuts::FileInputPanel::Panel"));

// end Panel

	dlog->AddPanel(this, container);

	itsFileRequiredCB->SetState(required);
	itsAllowInvalidFileCB->SetState(invalid);
	itsRequireFileReadCB->SetState(read);
	itsRequireFileWriteCB->SetState(write);
	itsRequireFileExecCB->SetState(exec);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileInputPanel::~FileInputPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
FileInputPanel::GetValues
	(
	bool* required,
	bool* invalid,
	bool* read,
	bool* write,
	bool* exec
	)
{
	*required = itsFileRequiredCB->IsChecked();
	*invalid  = itsAllowInvalidFileCB->IsChecked();
	*read     = itsRequireFileReadCB->IsChecked();
	*write    = itsRequireFileWriteCB->IsChecked();
	*exec     = itsRequireFileExecCB->IsChecked();
}
