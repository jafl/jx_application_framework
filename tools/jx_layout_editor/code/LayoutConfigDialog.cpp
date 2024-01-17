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
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXAtMostOneCBGroup.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

enum
{
	kWindowContainer = 1,
	kCustomContainer
};

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutConfigDialog::LayoutConfigDialog
	(
	const JString&	codeTag,
	const JString&	windowTitle,
	const JString&	containerName,
	const bool		adjustToFit
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(codeTag, windowTitle, containerName, adjustToFit);
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
	const JString&	codeTag,
	const JString&	windowTitle,
	const JString&	containerName,
	const bool		adjustToFit
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,180, JString::empty);

	itsLayoutTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 440,130);

	auto* codeTagLabel =
		jnew JXStaticText(JGetString("codeTagLabel::LayoutConfigDialog::JXLayout"),itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 130,20);
	codeTagLabel->SetToLabel(false);

	itsWindowTitleRB =
		jnew JXTextRadioButton(kWindowContainer, JGetString("itsWindowTitleRB::LayoutConfigDialog::JXLayout"),itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 130,20);
	itsWindowTitleRB->SetShortcuts(JGetString("itsWindowTitleRB::shortcuts::LayoutConfigDialog::JXLayout"));

	itsCustomContainerRB =
		jnew JXTextRadioButton(kCustomContainer, JGetString("itsCustomContainerRB::LayoutConfigDialog::JXLayout"),itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 130,20);
	itsCustomContainerRB->SetShortcuts(JGetString("itsCustomContainerRB::shortcuts::LayoutConfigDialog::JXLayout"));

	itsAdjustContentCB =
		jnew JXTextCheckbox(JGetString("itsAdjustContentCB::LayoutConfigDialog::JXLayout"),itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,100, 210,20);
	itsAdjustContentCB->SetShortcuts(JGetString("itsAdjustContentCB::shortcuts::LayoutConfigDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::LayoutConfigDialog::JXLayout"),window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 120,150, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::LayoutConfigDialog::JXLayout"),window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 289,149, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::LayoutConfigDialog::JXLayout"));

	itsCodeTagInput =
		jnew JXInputField(itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,10, 290,20);
	itsCodeTagInput->SetIsRequired();
	itsCodeTagInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9]+$", "i"), "itsCodeTagInput::validation::LayoutConfigDialog::JXLayout");

	itsWindowTitleInput =
		jnew JXInputField(itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,40, 290,20);

	itsContainerInput =
		jnew JXInputField(itsLayoutTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 140,70, 290,20);

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::LayoutConfigDialog"));
	SetButtons(okButton, cancelButton);

	itsCodeTagInput->GetText()->SetText(codeTag);

	itsLayoutTypeRG->SetBorderWidth(0);
	itsLayoutTypeRG->SelectItem(
		!containerName.IsEmpty() ? kCustomContainer : kWindowContainer);

	itsWindowTitleInput->GetText()->SetText(windowTitle);
	itsContainerInput->GetText()->SetText(containerName);
	itsAdjustContentCB->SetState(adjustToFit);

	UpdateDisplay();

	ListenTo(itsLayoutTypeRG, std::function([this](const JXRadioGroup::SelectionChanged&)
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
	if (itsLayoutTypeRG->GetSelectedItem() == kWindowContainer)
	{
		itsWindowTitleInput->Activate();

		itsContainerInput->Deactivate();
		itsContainerInput->SetIsRequired(false);
		itsAdjustContentCB->Deactivate();
	}
	else
	{
		itsWindowTitleInput->Deactivate();

		itsContainerInput->Activate();
		itsContainerInput->SetIsRequired();
		itsAdjustContentCB->Activate();
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Only validate container name if option is selected

 ******************************************************************************/

static const JRegex idPattern("^[_a-z][_a-z0-9]+$", "i");

bool
LayoutConfigDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	if (itsLayoutTypeRG->GetSelectedItem() == kCustomContainer &&
		!idPattern.Match(itsContainerInput->GetText()->GetText()))
	{
		JGetUserNotification()->ReportError(
			JGetString("ContainerNameMustBeValidIdentifier::LayoutConfigDialog"));
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
LayoutConfigDialog::GetConfig
	(
	JString*	codeTag,
	JString*	windowTitle,
	JString*	containerName,
	bool*		adjustToFit
	)
	const
{
	*codeTag = itsCodeTagInput->GetText()->GetText();

	if (itsLayoutTypeRG->GetSelectedItem() == kWindowContainer)
	{
		*windowTitle = itsWindowTitleInput->GetText()->GetText();
		containerName->Clear();
		*adjustToFit = false;
	}
	else
	{
		windowTitle->Clear();
		*containerName = itsContainerInput->GetText()->GetText();
		*adjustToFit   = itsAdjustContentCB->IsChecked();
	}
}
