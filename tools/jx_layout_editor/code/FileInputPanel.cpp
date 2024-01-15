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

	const bool invalid,
	const bool read,
	const bool write,
	const bool exec
	)
{
	BuildPanel(dlog, invalid, read, write, exec);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileInputPanel::~FileInputPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
FileInputPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

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

	itsAllowInvalidFileCB =
		jnew JXTextCheckbox(JGetString("itsAllowInvalidFileCB::FileInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 130,20);
	itsAllowInvalidFileCB->SetShortcuts(JGetString("itsAllowInvalidFileCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileReadCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileReadCB::FileInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,10, 60,20);
	itsRequireFileReadCB->SetShortcuts(JGetString("itsRequireFileReadCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileWriteCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileWriteCB::FileInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,10, 60,20);
	itsRequireFileWriteCB->SetShortcuts(JGetString("itsRequireFileWriteCB::shortcuts::FileInputPanel::Panel"));

	itsRequireFileExecCB =
		jnew JXTextCheckbox(JGetString("itsRequireFileExecCB::FileInputPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 380,10, 60,20);
	itsRequireFileExecCB->SetShortcuts(JGetString("itsRequireFileExecCB::shortcuts::FileInputPanel::Panel"));

// end Panel

	dlog->AddPanel(this, container);

	itsAllowInvalidFileCB->SetState(invalid);
	itsRequireFileReadCB->SetState(read);
	itsRequireFileWriteCB->SetState(write);
	itsRequireFileExecCB->SetState(exec);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
FileInputPanel::GetValues
	(
	bool* invalid,
	bool* read,
	bool* write,
	bool* exec
	)
{
	*invalid = itsAllowInvalidFileCB->IsChecked();
	*read    = itsRequireFileReadCB->IsChecked();
	*write   = itsRequireFileWriteCB->IsChecked();
	*exec    = itsRequireFileExecCB->IsChecked();
}
