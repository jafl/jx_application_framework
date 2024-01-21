/******************************************************************************
 InputFieldPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "InputFieldPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXRegexInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

InputFieldPanel::InputFieldPanel
	(
	WidgetParametersDialog* dlog,

	const bool		required,
	const JInteger	minLength,
	const JInteger	maxLength,
	const JString&	regexPattern,
	const JString&	regexFlags,
	const JString&	regexErrorMsg,
	const bool		wordWrap,
	const bool		newlines
	)
{
	auto* window    = dlog->GetWindow();
	auto* testRegex = jnew JRegex;

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,120);
	assert( container != nullptr );

	itsValueRequiredCB =
		jnew JXTextCheckbox(JGetString("itsValueRequiredCB::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	itsValueRequiredCB->SetShortcuts(JGetString("itsValueRequiredCB::shortcuts::InputFieldPanel::Panel"));

	auto* minimumLengthLabel =
		jnew JXStaticText(JGetString("minimumLengthLabel::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 70,20);
	minimumLengthLabel->SetToLabel(false);

	auto* maximumLengthLabel =
		jnew JXStaticText(JGetString("maximumLengthLabel::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,10, 80,20);
	maximumLengthLabel->SetToLabel(false);

	auto* regExpValidationLabel =
		jnew JXStaticText(JGetString("regExpValidationLabel::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 100,20);
	regExpValidationLabel->SetToLabel(false);

	auto* regexFlagsLabel =
		jnew JXStaticText(JGetString("regexFlagsLabel::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,40, 40,20);
	regexFlagsLabel->SetToLabel(false);

	auto* regexpErrorMessageLabel =
		jnew JXStaticText(JGetString("regexpErrorMessageLabel::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 100,20);
	regexpErrorMessageLabel->SetToLabel(false);

	itsWordWrapCB =
		jnew JXTextCheckbox(JGetString("itsWordWrapCB::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 180,20);
	itsWordWrapCB->SetShortcuts(JGetString("itsWordWrapCB::shortcuts::InputFieldPanel::Panel"));

	itsAllowNewlinesCB =
		jnew JXTextCheckbox(JGetString("itsAllowNewlinesCB::InputFieldPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,90, 120,20);
	itsAllowNewlinesCB->SetShortcuts(JGetString("itsAllowNewlinesCB::shortcuts::InputFieldPanel::Panel"));

	itsMinLengthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,10, 40,20);

	itsMaxLengthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,10, 40,20);

	itsRegexInput =
		jnew JXRegexInput(testRegex, true,container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,40, 240,20);

	itsRegexFlagsInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,40, 40,20);

	itsRegexErrorMsgInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,60, 321,20);

// end Panel

	dlog->AddPanel(this, container);

	itsValueRequiredCB->SetState(required);

	ConfigureInput(itsMinLengthInput, minLength);
	ConfigureInput(itsMaxLengthInput, maxLength);

	itsRegexInput->SetIsRequired(false);
	itsRegexInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsRegexInput->GetText()->SetText(regexPattern);

	itsRegexFlagsInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsRegexFlagsInput->GetText()->SetText(regexFlags);

	itsRegexErrorMsgInput->GetText()->SetText(regexErrorMsg);

	ListenTo(itsRegexInput->GetText(), std::function([this](const JStyledText::TextSet&)
	{
		UpdateDisplay();
	}));

	ListenTo(itsRegexInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		UpdateDisplay();
	}));

	itsWordWrapCB->SetState(wordWrap);
	itsAllowNewlinesCB->SetState(newlines);

	UpdateDisplay();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

InputFieldPanel::~InputFieldPanel()
{
}

/******************************************************************************
 ConfigureInput (private)

 ******************************************************************************/

void
InputFieldPanel::ConfigureInput
	(
	JXIntegerInput*	field,
	const JInteger	value
	)
{
	field->SetIsRequired(false);
	if (value > 0)
	{
		field->SetValue(value);
	}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
InputFieldPanel::UpdateDisplay()
	const
{
	itsRegexErrorMsgInput->SetIsRequired(!itsRegexInput->GetText()->IsEmpty());
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
InputFieldPanel::Validate()
	const
{
	JInteger min, max;
	if (itsMinLengthInput->GetValue(&min) &&
		itsMaxLengthInput->GetValue(&max) &&
		max < min)
	{
		JGetUserNotification()->ReportError(
			JGetString("MinLessThanMax::InputFieldPanel"));
		JXFocusWidgetTask::Focus(itsMinLengthInput);
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
InputFieldPanel::GetValues
	(
	bool*		required,
	JInteger*	minLength,
	JInteger*	maxLength,
	JString*	regexPattern,
	JString*	regexFlags,
	JString*	regexErrorMsg,
	bool*		wordWrap,
	bool*		newlines
	)
{
	*required      = itsValueRequiredCB->IsChecked();
	*regexPattern  = itsRegexInput->GetText()->GetText();
	*regexFlags    = itsRegexFlagsInput->GetText()->GetText();
	*regexErrorMsg = itsRegexErrorMsgInput->GetText()->GetText();

	itsMinLengthInput->GetValue(minLength);
	itsMaxLengthInput->GetValue(maxLength);

	*wordWrap = itsWordWrapCB->IsChecked();
	*newlines = itsAllowNewlinesCB->IsChecked();
}
