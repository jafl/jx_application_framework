/******************************************************************************
 RadioButtonIDPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "RadioButtonIDPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXFocusWidgetTask.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

RadioButtonIDPanel::RadioButtonIDPanel
	(
	WidgetParametersDialog* dlog,

	const JString& id
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	auto* idLabel =
		jnew JXStaticText(JGetString("idLabel::RadioButtonIDPanel::Panel"),container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	idLabel->SetToLabel(false);

	itsIDInput =
		jnew JXInputField(false, true,container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,10, 340,20);
	itsIDInput->SetIsRequired();
	itsIDInput->SetValidationPattern(jnew JRegex("^([_a-z][_a-z0-9]+|[1-9][0-9]*)$", "i"), "itsIDInput::validation::RadioButtonIDPanel::Panel");

// end Panel

	dlog->AddPanel(this, container);

	itsIDInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsIDInput->GetText()->SetText(id);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RadioButtonIDPanel::~RadioButtonIDPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
RadioButtonIDPanel::GetValues
	(
	JString* id
	)
{
	*id = itsIDInput->GetText()->GetText();
}
