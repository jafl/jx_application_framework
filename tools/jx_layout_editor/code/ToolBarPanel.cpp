/******************************************************************************
 ToolBarPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ToolBarPanel.h"
#include "WidgetParametersDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ToolBarPanel::ToolBarPanel
	(
	WidgetParametersDialog* dlog,

	const JString&	prefsMgr,
	const JString&	prefID,
	const JString&	menuBar
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	auto* prefsManagerLabel =
		jnew JXStaticText(JGetString("prefsManagerLabel::ToolBarPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 100,20);
	prefsManagerLabel->SetToLabel(false);

	auto* prefIDLabel =
		jnew JXStaticText(JGetString("prefIDLabel::ToolBarPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 100,20);
	prefIDLabel->SetToLabel(false);

	auto* menuBarLabel =
		jnew JXStaticText(JGetString("menuBarLabel::ToolBarPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 100,20);
	menuBarLabel->SetToLabel(false);

	itsPrefsMgrInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,10, 320,20);
	itsPrefsMgrInput->SetIsRequired();
	itsPrefsMgrInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:\\(\\)]+$", "i"), "itsPrefsMgrInput::validation::ToolBarPanel::Panel");

	itsPrefIDInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,40, 320,20);
	itsPrefIDInput->SetIsRequired();
	itsPrefIDInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:]+$", "i"), "itsPrefIDInput::validation::ToolBarPanel::Panel");

	itsMenuBarInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,70, 320,20);
	itsMenuBarInput->SetIsRequired();
	itsMenuBarInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:\\(\\)]+$", "i"), "itsMenuBarInput::validation::ToolBarPanel::Panel");

// end Panel

	dlog->AddPanel(this, container);

	itsPrefsMgrInput->GetText()->SetText(prefsMgr);
	itsPrefIDInput->GetText()->SetText(prefID);
	itsMenuBarInput->GetText()->SetText(menuBar);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ToolBarPanel::~ToolBarPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
ToolBarPanel::GetValues
	(
	JString* prefsMgr,
	JString* prefID,
	JString* menuBar
	)
{
	*prefsMgr = itsPrefsMgrInput->GetText()->GetText();
	*prefID   = itsPrefIDInput->GetText()->GetText();
	*menuBar  = itsMenuBarInput->GetText()->GetText();
}
