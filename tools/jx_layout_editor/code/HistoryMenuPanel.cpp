/******************************************************************************
 HistoryMenuPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "HistoryMenuPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

HistoryMenuPanel::HistoryMenuPanel
	(
	WidgetParametersDialog* dlog,

	const JString& length
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	auto* historyLengthLabel =
		jnew JXStaticText(JGetString("historyLengthLabel::HistoryMenuPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 100,20);
	historyLengthLabel->SetToLabel(false);

	itsHistoryLengthInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 320,20);
	itsHistoryLengthInput->SetValidationPattern(jnew JRegex("^([_a-z][_a-z0-9]+|[1-9][0-9]*)$", "i"), "itsHistoryLengthInput::validation::HistoryMenuPanel::Panel");

// end Panel

	dlog->AddPanel(this, container);

	itsHistoryLengthInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsHistoryLengthInput->GetText()->SetText(length);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

HistoryMenuPanel::~HistoryMenuPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
HistoryMenuPanel::GetValues
	(
	JString* length
	)
{
	*length = itsHistoryLengthInput->GetText()->GetText();
}
