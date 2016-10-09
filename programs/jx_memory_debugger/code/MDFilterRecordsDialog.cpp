/******************************************************************************
 MDFilterRecordsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "MDFilterRecordsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXStaticText.h>
#include <JXIntegerInput.h>
#include <JXAtLeastOneCBGroup.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDFilterRecordsDialog::MDFilterRecordsDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDFilterRecordsDialog::~MDFilterRecordsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
MDFilterRecordsDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 420,120, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::MDFilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,90, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::MDFilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,90, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::MDFilterRecordsDialog::shortcuts::JXLayout"));

	itsFileCB =
		jnew JXTextCheckbox(JGetString("itsFileCB::MDFilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 130,20);
	assert( itsFileCB != NULL );
	itsFileCB->SetShortcuts(JGetString("itsFileCB::MDFilterRecordsDialog::shortcuts::JXLayout"));

	itsSizeCB =
		jnew JXTextCheckbox(JGetString("itsSizeCB::MDFilterRecordsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 130,20);
	assert( itsSizeCB != NULL );
	itsSizeCB->SetShortcuts(JGetString("itsSizeCB::MDFilterRecordsDialog::shortcuts::JXLayout"));

	itsFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,20, 250,20);
	assert( itsFileInput != NULL );

	itsSizeInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 150,50, 250,20);
	assert( itsSizeInput != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDFilterRecordsDialog"));
	SetButtons(okButton, cancelButton);

	JXAtLeastOneCBGroup* cbGroup = jnew JXAtLeastOneCBGroup(2, itsSizeCB, itsFileCB);
	assert( cbGroup != NULL );

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
MDFilterRecordsDialog::Receive
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
	else if (sender == itsSizeInput && message.Is(JTextEditor::kTextChanged))
		{
		itsSizeCB->SetState(kJTrue);
		}

	else if (sender == itsFileCB && message.Is(JXCheckbox::kPushed))
		{
		if (itsFileCB->IsChecked())
			{
			itsFileInput->Focus();
			}
		}
	else if (sender == itsFileInput && message.Is(JTextEditor::kTextChanged))
		{
		itsFileCB->SetState(kJTrue);
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
MDFilterRecordsDialog::BuildFilter
	(
	JMemoryManager::RecordFilter* filter
	)
	const
{
	if (itsSizeCB->IsChecked())
		{
		JInteger minSize;
		const JBoolean ok = itsSizeInput->GetValue(&minSize);
		assert( ok );
		filter->minSize = minSize;
		}

	if (itsFileCB->IsChecked())
		{
		jdelete filter->fileName;
		filter->fileName = jnew JString(itsFileInput->GetText());
		assert( filter->fileName != NULL );
		}
}
