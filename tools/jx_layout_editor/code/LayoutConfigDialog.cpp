/******************************************************************************
 LayoutConfigDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutConfigDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXAtMostOneCBGroup.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutConfigDialog::LayoutConfigDialog
	(
	const JString&	windowTitle,
	const JString&	containerName,
	const JString&	codeTag,
	const bool		adjustToFit
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(windowTitle, containerName, codeTag, adjustToFit);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutConfigDialog::~LayoutConfigDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
LayoutConfigDialog::BuildWindow
	(
	const JString&	windowTitle,
	const JString&	containerName,
	const JString&	codeTag,
	const bool		adjustToFit
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,180, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 290,150, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::LayoutConfigDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 120,150, 60,20);
	assert( cancelButton != nullptr );

	itsWindowTitleCB =
		jnew JXTextCheckbox(JGetString("itsWindowTitleCB::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 130,20);
	assert( itsWindowTitleCB != nullptr );
	itsWindowTitleCB->SetShortcuts(JGetString("itsWindowTitleCB::LayoutConfigDialog::shortcuts::JXLayout"));

	itsCustomContainerCB =
		jnew JXTextCheckbox(JGetString("itsCustomContainerCB::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 130,20);
	assert( itsCustomContainerCB != nullptr );
	itsCustomContainerCB->SetShortcuts(JGetString("itsCustomContainerCB::LayoutConfigDialog::shortcuts::JXLayout"));

	itsWindowTitleInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 150,20, 290,20);
	assert( itsWindowTitleInput != nullptr );

	itsContainerInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 150,50, 290,20);
	assert( itsContainerInput != nullptr );

	auto* codeTagLabel =
		jnew JXStaticText(JGetString("codeTagLabel::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,80, 110,20);
	assert( codeTagLabel != nullptr );
	codeTagLabel->SetToLabel();

	itsCodeTagInput =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 150,80, 290,20);
	assert( itsCodeTagInput != nullptr );

	itsAdjustContentCB =
		jnew JXTextCheckbox(JGetString("itsAdjustContentCB::LayoutConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,110, 210,20);
	assert( itsAdjustContentCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::LayoutConfigDialog"));
	SetButtons(okButton, cancelButton);

	jnew JXAtMostOneCBGroup(2, itsWindowTitleCB, itsCustomContainerCB);
	if (!containerName.IsEmpty())
	{
		itsCustomContainerCB->SetState(true);
	}
	else
	{
		itsWindowTitleCB->SetState(true);
	}

	itsWindowTitleInput->GetText()->SetText(windowTitle);
	itsContainerInput->GetText()->SetText(containerName);
	itsCodeTagInput->GetText()->SetText(codeTag);
	itsAdjustContentCB->SetState(adjustToFit);

	UpdateDisplay();

	ListenTo(itsWindowTitleCB, std::function([this](const JXCheckbox::Pushed&)
	{
		UpdateDisplay();
	}));

	ListenTo(itsCustomContainerCB, std::function([this](const JXCheckbox::Pushed&)
	{
		UpdateDisplay();
	}));
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
LayoutConfigDialog::UpdateDisplay()
{
	if (itsWindowTitleCB->IsChecked())
	{
		itsWindowTitleInput->Activate();
		itsWindowTitleInput->SetIsRequired();

		itsContainerInput->Deactivate();
		itsContainerInput->SetIsRequired(false);
		itsCodeTagInput->Deactivate();
		itsCodeTagInput->SetIsRequired(false);
		itsAdjustContentCB->Deactivate();
	}
	else
	{
		itsWindowTitleInput->Deactivate();
		itsWindowTitleInput->SetIsRequired(false);

		itsContainerInput->Activate();
		itsContainerInput->SetIsRequired();
		itsCodeTagInput->Activate();
		itsCodeTagInput->SetIsRequired();
		itsAdjustContentCB->Activate();
	}
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
LayoutConfigDialog::GetConfig
	(
	JString*	windowTitle,
	JString*	containerName,
	JString*	codeTag,
	bool*		adjustToFit
	)
	const
{
	if (itsWindowTitleCB->IsChecked())
	{
		*windowTitle = itsWindowTitleInput->GetText()->GetText();
		containerName->Clear();
		codeTag->Clear();
		*adjustToFit = false;
	}
	else
	{
		windowTitle->Clear();
		*containerName = itsContainerInput->GetText()->GetText();
		*codeTag       = itsCodeTagInput->GetText()->GetText();
		*adjustToFit   = itsAdjustContentCB->IsChecked();
	}
}
