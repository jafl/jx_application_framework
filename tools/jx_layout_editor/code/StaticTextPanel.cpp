/******************************************************************************
 StaticTextPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "StaticTextPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXRegexInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

StaticTextPanel::StaticTextPanel
	(
	WidgetParametersDialog* dlog,

	const JString&	text,
	const bool		label,
	const bool		center,
	const bool		wordWrap,
	const bool		selectable,
	const bool		styleable
	)
{
	auto* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,130);
	assert( container != nullptr );

	auto* textLabel =
		jnew JXStaticText(JGetString("textLabel::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 40,20);
	textLabel->SetToLabel(false);

	itsLabelCB =
		jnew JXTextCheckbox(JGetString("itsLabelCB::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,10, 60,20);
	itsLabelCB->SetShortcuts(JGetString("itsLabelCB::shortcuts::StaticTextPanel::Panel"));

	itsCenterHorizCB =
		jnew JXTextCheckbox(JGetString("itsCenterHorizCB::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,10, 80,20);
	itsCenterHorizCB->SetShortcuts(JGetString("itsCenterHorizCB::shortcuts::StaticTextPanel::Panel"));

	itsWordWrapCB =
		jnew JXTextCheckbox(JGetString("itsWordWrapCB::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 170,20);
	itsWordWrapCB->SetShortcuts(JGetString("itsWordWrapCB::shortcuts::StaticTextPanel::Panel"));

	itsSelectableCB =
		jnew JXTextCheckbox(JGetString("itsSelectableCB::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,100, 90,20);
	itsSelectableCB->SetShortcuts(JGetString("itsSelectableCB::shortcuts::StaticTextPanel::Panel"));

	itsStyleableCB =
		jnew JXTextCheckbox(JGetString("itsStyleableCB::StaticTextPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,100, 80,20);
	itsStyleableCB->SetShortcuts(JGetString("itsStyleableCB::shortcuts::StaticTextPanel::Panel"));

	itsTextInput =
		jnew JXInputField(true, true, container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,30, 420,60);

// end Panel

	dlog->AddPanel(this, container);

	itsTextInput->GetText()->SetText(text);
	itsLabelCB->SetState(label);
	itsCenterHorizCB->SetState(center);
	itsWordWrapCB->SetState(wordWrap);
	itsSelectableCB->SetState(selectable);
	itsStyleableCB->SetState(styleable);

	ListenTo(itsLabelCB, std::function([this](const JXCheckbox::Pushed&)
	{
		UpdateDisplay();
	}));

	UpdateDisplay();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StaticTextPanel::~StaticTextPanel()
{
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
StaticTextPanel::UpdateDisplay()
	const
{
	if (itsLabelCB->IsChecked())
	{
		itsCenterHorizCB->Activate();
		itsWordWrapCB->Deactivate();
		itsWordWrapCB->SetState(false);
	}
	else
	{
		itsCenterHorizCB->Deactivate();
		itsCenterHorizCB->SetState(false);
		itsWordWrapCB->Activate();
	}
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
StaticTextPanel::GetValues
	(
	JString*	text,
	bool*		label,
	bool*		center,
	bool*		wordWrap,
	bool*		selectable,
	bool*		styleable
	)
{
	text->Set(itsTextInput->GetText()->GetText());
	*label      = itsLabelCB->IsChecked();
	*center     = itsCenterHorizCB->IsChecked();
	*wordWrap   = itsWordWrapCB->IsChecked();
	*selectable = itsSelectableCB->IsChecked();
	*styleable  = itsStyleableCB->IsChecked();
}
