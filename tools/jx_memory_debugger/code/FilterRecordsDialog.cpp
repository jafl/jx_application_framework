/******************************************************************************
 FilterRecordsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "FilterRecordsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FilterRecordsDialog::FilterRecordsDialog
	(
	const JMemoryManager::RecordFilter& filter
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(filter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FilterRecordsDialog::~FilterRecordsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
FilterRecordsDialog::BuildWindow
	(
	const JMemoryManager::RecordFilter& filter
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 420,120, JGetString("WindowTitle::FilterRecordsDialog::JXLayout"));

	itsFileCB =
		jnew JXTextCheckbox(JGetString("itsFileCB::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 130,20);
	itsFileCB->SetShortcuts(JGetString("itsFileCB::shortcuts::FilterRecordsDialog::JXLayout"));

	itsSizeCB =
		jnew JXTextCheckbox(JGetString("itsSizeCB::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 130,20);
	itsSizeCB->SetShortcuts(JGetString("itsSizeCB::shortcuts::FilterRecordsDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,90, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,90, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::FilterRecordsDialog::JXLayout"));

	itsFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,20, 250,20);

	itsSizeInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,50, 250,20);
	itsSizeInput->SetIsRequired(false);
	itsSizeInput->SetLowerLimit(0);

// end JXLayout

	SetButtons(okButton, cancelButton);

	if (filter.fileName != nullptr)
	{
		itsFileCB->SetState(true);
		itsFileInput->GetText()->SetText(*filter.fileName);
	}

	if (filter.minSize > 0)
	{
		itsSizeCB->SetState(true);
		itsSizeInput->GetText()->SetText(JString(filter.minSize - 1));
	}

	ListenTo(itsFileCB, std::function([this](const JXCheckbox::Pushed& msg)
	{
		if (msg.IsChecked())
		{
			itsFileInput->Focus();
		}
	}));

	ListenTo(itsFileInput, std::function([this](const JStyledText::TextChanged&)
	{
		itsFileCB->SetState(true);
	}));

	ListenTo(itsSizeCB, std::function([this](const JXCheckbox::Pushed& msg)
	{
		if (msg.IsChecked())
		{
			itsSizeInput->Focus();
		}
	}));

	ListenTo(itsSizeInput, std::function([this](const JStyledText::TextChanged&)
	{
		itsSizeCB->SetState(true);
	}));
}

/******************************************************************************
 BuildFilter

 ******************************************************************************/

void
FilterRecordsDialog::BuildFilter
	(
	JMemoryManager::RecordFilter* filter
	)
	const
{
	filter->minSize = 0;

	if (itsSizeCB->IsChecked())
	{
		JInteger minSize;
		const bool ok = itsSizeInput->GetValue(&minSize);
		assert( ok );
		if (minSize > 0)
		{
			filter->minSize = minSize + 1;
		}
	}

	jdelete filter->fileName;
	filter->fileName = nullptr;

	if (itsFileCB->IsChecked())
	{
		filter->fileName = jnew JString(itsFileInput->GetText()->GetText());
	}
}
