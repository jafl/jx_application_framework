/******************************************************************************
 InputFieldPanel.cpp

	BASE CLASS = JBroadcaster

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
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JRegex.h>
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
	const JString&	regexErrorMsg
	)
{
	BuildPanel(dlog, required, minLength, maxLength, regexPattern, regexErrorMsg);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

InputFieldPanel::~InputFieldPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
InputFieldPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	const bool		required,
	const JInteger	minLength,
	const JInteger	maxLength,
	const JString&	regexPattern,
	const JString&	regexErrorMsg
	)
{
	auto* window    = dlog->GetWindow();
	auto* testRegex = jnew JRegex;

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	itsValueRequiredCB =
		jnew JXTextCheckbox(JGetString("itsValueRequiredCB::InputFieldPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	assert( itsValueRequiredCB != nullptr );
	itsValueRequiredCB->SetShortcuts(JGetString("itsValueRequiredCB::InputFieldPanel::shortcuts::Panel"));

	itsMinLengthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 190,10, 40,20);
	assert( itsMinLengthInput != nullptr );

	auto* minimumLengthLabel =
		jnew JXStaticText(JGetString("minimumLengthLabel::InputFieldPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 70,20);
	assert( minimumLengthLabel != nullptr );
	minimumLengthLabel->SetToLabel();

	itsMaxLengthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,10, 40,20);
	assert( itsMaxLengthInput != nullptr );

	auto* maximumLengthLabel =
		jnew JXStaticText(JGetString("maximumLengthLabel::InputFieldPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,10, 80,20);
	assert( maximumLengthLabel != nullptr );
	maximumLengthLabel->SetToLabel();

	auto* regExpValidationLabel =
		jnew JXStaticText(JGetString("regExpValidationLabel::InputFieldPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 100,20);
	assert( regExpValidationLabel != nullptr );
	regExpValidationLabel->SetToLabel();

	auto* regexpErrorMessageLabe =
		jnew JXStaticText(JGetString("regexpErrorMessageLabe::InputFieldPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 100,20);
	assert( regexpErrorMessageLabe != nullptr );
	regexpErrorMessageLabe->SetToLabel();

	itsRegexInput =
		jnew JXRegexInput(testRegex, true, container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,50, 320,20);
	assert( itsRegexInput != nullptr );

	itsRegexErrorMsgInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,70, 321,20);
	assert( itsRegexErrorMsgInput != nullptr );

// end Panel

	dlog->AddPanel(container);

	itsValueRequiredCB->SetState(required);

	ConfigureInput(itsMinLengthInput, minLength);
	ConfigureInput(itsMaxLengthInput, maxLength);

	itsRegexInput->SetIsRequired(false);
	itsRegexInput->GetText()->SetText(regexPattern);
	itsRegexErrorMsgInput->GetText()->SetText(regexErrorMsg);

	ListenTo(itsRegexInput->GetText(), std::function([this](const JStyledText::TextSet&)
	{
		itsRegexErrorMsgInput->SetIsRequired(!itsRegexInput->GetText()->IsEmpty());
	}));

	ListenTo(itsRegexInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		itsRegexErrorMsgInput->SetIsRequired(!itsRegexInput->GetText()->IsEmpty());
	}));
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
	else
	{
		field->GetText()->SetText(JString::empty);
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
	JString*	regexErrorMsg
	)
{
	*required      = itsValueRequiredCB->IsChecked();
	*regexPattern  = itsRegexInput->GetText()->GetText();
	*regexErrorMsg = itsRegexErrorMsgInput->GetText()->GetText();

	itsMinLengthInput->GetValue(minLength);
	itsMaxLengthInput->GetValue(maxLength);
}
