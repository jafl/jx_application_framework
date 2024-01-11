/******************************************************************************
 WidgetLabelPanel.cpp

	BASE CLASS = WidgetPanelBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetLabelPanel.h"
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

WidgetLabelPanel::WidgetLabelPanel
	(
	WidgetParametersDialog* dlog,

	const JString& label,
	const JString& shortcuts
	)
{
	BuildPanel(dlog, label, shortcuts);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WidgetLabelPanel::~WidgetLabelPanel()
{
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

void
WidgetLabelPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	const JString&	label,
	const JString&	shortcuts
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,70);
	assert( container != nullptr );

	itsLabelInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,10, 340,20);
	assert( itsLabelInput != nullptr );

	auto* widgetLabelLabel =
		jnew JXStaticText(JGetString("widgetLabelLabel::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	assert( widgetLabelLabel != nullptr );
	widgetLabelLabel->SetToLabel();

	itsShortcutsInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,40, 60,20);
	assert( itsShortcutsInput != nullptr );

	auto* shortcutsLabel =
		jnew JXStaticText(JGetString("shortcutsLabel::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 80,20);
	assert( shortcutsLabel != nullptr );
	shortcutsLabel->SetToLabel();

	auto* shortcutsHelp =
		jnew JXStaticText(JGetString("shortcutsHelp::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,40, 270,20);
	assert( shortcutsHelp != nullptr );
	shortcutsHelp->SetToLabel();

// end Panel

	dlog->AddPanel(this, container);

	itsLabelInput->SetIsRequired();
	itsLabelInput->GetText()->SetText(label);

	itsShortcutsInput->GetText()->SetText(shortcuts);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
WidgetLabelPanel::GetValues
	(
	JString* label,
	JString* shortcuts
	)
{
	*label     = itsLabelInput->GetText()->GetText();
	*shortcuts = itsShortcutsInput->GetText()->GetText();
}
