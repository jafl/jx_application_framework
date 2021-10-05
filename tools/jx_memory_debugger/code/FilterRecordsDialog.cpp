/******************************************************************************
 FilterRecordsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "FilterRecordsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXAtLeastOneCBGroup.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FilterRecordsDialog::FilterRecordsDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow();
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
FilterRecordsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 420,120, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,90, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,90, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::FilterRecordsDialog::shortcuts::JXLayout"));

	itsFileCB =
		jnew JXTextCheckbox(JGetString("itsFileCB::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 130,20);
	assert( itsFileCB != nullptr );
	itsFileCB->SetShortcuts(JGetString("itsFileCB::FilterRecordsDialog::shortcuts::JXLayout"));

	itsSizeCB =
		jnew JXTextCheckbox(JGetString("itsSizeCB::FilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 130,20);
	assert( itsSizeCB != nullptr );
	itsSizeCB->SetShortcuts(JGetString("itsSizeCB::FilterRecordsDialog::shortcuts::JXLayout"));

	itsFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,20, 250,20);
	assert( itsFileInput != nullptr );

	itsSizeInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,50, 250,20);
	assert( itsSizeInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::FilterRecordsDialog"));
	SetButtons(okButton, cancelButton);

	auto* cbGroup = jnew JXAtLeastOneCBGroup(2, itsSizeCB, itsFileCB);
	assert( cbGroup != nullptr );

	ListenTo(itsFileCB);
	ListenTo(itsFileInput);

	ListenTo(itsSizeCB);
	ListenTo(itsSizeInput);
	itsSizeInput->SetLowerLimit(0);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FilterRecordsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSizeCB && message.Is(JXCheckbox::kPushed))
	{
		if (itsSizeCB->IsChecked())
		{
			itsSizeInput->Focus();
		}
	}
	else if (sender == itsSizeInput && message.Is(JStyledText::kTextChanged))
	{
		itsSizeCB->SetState(true);
	}

	else if (sender == itsFileCB && message.Is(JXCheckbox::kPushed))
	{
		if (itsFileCB->IsChecked())
		{
			itsFileInput->Focus();
		}
	}
	else if (sender == itsFileInput && message.Is(JStyledText::kTextChanged))
	{
		itsFileCB->SetState(true);
	}

	else
	{
		JXDialogDirector::Receive(sender, message);
	}
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
	if (itsSizeCB->IsChecked())
	{
		JInteger minSize;
		const bool ok = itsSizeInput->GetValue(&minSize);
		assert( ok );
		filter->minSize = minSize;
	}

	if (itsFileCB->IsChecked())
	{
		jdelete filter->fileName;
		filter->fileName = jnew JString(itsFileInput->GetText()->GetText());
		assert( filter->fileName != nullptr );
	}
}
