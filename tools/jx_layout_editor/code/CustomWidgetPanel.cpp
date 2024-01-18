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
	const bool		wantsInput
	)
{
	BuildPanel(dlog, className, ctorArgs, needsCreate, wantsInput);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CustomWidgetPanel::~CustomWidgetPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
CustomWidgetPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	const JString&	className,
	const JString&	ctorArgs,
	const bool		needsCreate,
	const bool		wantsInput
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::CustomWidgetPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	classNameLabel->SetToLabel(false);

	auto* constructorArgsLabel =
		jnew JXStaticText(JGetString("constructorArgsLabel::CustomWidgetPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,39, 80,20);
	constructorArgsLabel->SetToLabel(false);

	itsWantsInputCB =
		jnew JXTextCheckbox(JGetString("itsWantsInputCB::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 160,20);
	itsWantsInputCB->SetShortcuts(JGetString("itsWantsInputCB::shortcuts::CustomWidgetPanel::Panel"));

	itsNeedsCreateCB =
		jnew JXTextCheckbox(JGetString("itsNeedsCreateCB::CustomWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,70, 160,20);
	itsNeedsCreateCB->SetShortcuts(JGetString("itsNeedsCreateCB::shortcuts::CustomWidgetPanel::Panel"));

	itsClassNameInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,10, 340,20);

	itsCtorArgs =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,40, 340,20);

// end Panel

	dlog->AddPanel(this, container);

	itsClassNameInput->SetIsRequired();
	itsClassNameInput->SetValidationPattern(
		jnew JRegex("^[_a-z][_a-z0-9]+$", "i"),
		"ClassNameMustBeValid::CustomWidgetPanel");
	itsClassNameInput->GetText()->SetText(className);

	itsCtorArgs->GetText()->SetText(ctorArgs);
	itsNeedsCreateCB->SetState(needsCreate);

	itsWantsInputCB->SetState(wantsInput);
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
	bool*		wantsInput
	)
{
	*className   = itsClassNameInput->GetText()->GetText();
	*ctorArgs    = itsCtorArgs->GetText()->GetText();
	*needsCreate = itsNeedsCreateCB->IsChecked();
	*wantsInput  = itsWantsInputCB->IsChecked();
}
