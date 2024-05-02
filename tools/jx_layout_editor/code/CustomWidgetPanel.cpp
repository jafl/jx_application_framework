/******************************************************************************
 CustomWidgetPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "CustomWidgetPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CustomWidgetPanel::CustomWidgetPanel
	(
	WidgetParametersDialog* dlog,

	const JString&	className,
	const JString&	ctorArgs,
	const bool		needsCreate,
	const JString&	deps,
	const bool		wantsInput
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,130);

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	classNameLabel->SetToLabel(false);

	auto* constructorArgsLabel =
		jnew JXStaticText(JGetString("constructorArgsLabel::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,39, 80,20);
	constructorArgsLabel->SetToLabel(false);

	auto* dependencyLabel =
		jnew JXStaticText(JGetString("dependencyLabel::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 80,20);
	dependencyLabel->SetToLabel(false);

	itsWantsInputCB =
		jnew JXTextCheckbox(JGetString("itsWantsInputCB::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 160,20);
	itsWantsInputCB->SetShortcuts(JGetString("itsWantsInputCB::shortcuts::CustomWidgetPanel::Panel"));

	itsNeedsCreateCB =
		jnew JXTextCheckbox(JGetString("itsNeedsCreateCB::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,100, 160,20);
	itsNeedsCreateCB->SetShortcuts(JGetString("itsNeedsCreateCB::shortcuts::CustomWidgetPanel::Panel"));

	itsClassNameInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,10, 340,20);

	itsCtorArgs =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,40, 340,20);

	itsDependencyInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,70, 340,20);
	itsDependencyInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:,]+$", "i"), "itsDependencyInput::validation::CustomWidgetPanel::Panel");

// end Panel

	dlog->AddPanel(this, container);

	itsClassNameInput->SetIsRequired();
	itsClassNameInput->SetValidationPattern(
		jnew JRegex("^[_a-z][_a-z0-9:]+$", "i"),
		"ClassNameMustBeValid::CustomWidgetPanel");
	itsClassNameInput->GetText()->SetText(className);

	itsCtorArgs->GetText()->SetText(ctorArgs);
	itsNeedsCreateCB->SetState(needsCreate);

	itsDependencyInput->GetText()->SetText(deps);

	itsWantsInputCB->SetState(wantsInput);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CustomWidgetPanel::~CustomWidgetPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
CustomWidgetPanel::GetValues
	(
	JString*	className,
	JString*	ctorArgs,
	bool*		needsCreate,
	JString*	deps,
	bool*		wantsInput
	)
{
	*className   = itsClassNameInput->GetText()->GetText();
	*ctorArgs    = itsCtorArgs->GetText()->GetText();
	*deps        = itsDependencyInput->GetText()->GetText();
	*needsCreate = itsNeedsCreateCB->IsChecked();
	*wantsInput  = itsWantsInputCB->IsChecked();
}
