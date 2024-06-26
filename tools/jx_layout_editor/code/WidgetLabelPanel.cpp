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
#include <jx-af/jx/JXFocusWidgetTask.h>
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
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,70);

	auto* widgetLabelLabel =
		jnew JXStaticText(JGetString("widgetLabelLabel::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 80,20);
	widgetLabelLabel->SetToLabel(false);

	auto* shortcutsLabel =
		jnew JXStaticText(JGetString("shortcutsLabel::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 80,20);
	shortcutsLabel->SetToLabel(false);

	auto* shortcutsHelp =
		jnew JXStaticText(JGetString("shortcutsHelp::WidgetLabelPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,40, 270,20);
	shortcutsHelp->SetToLabel(false);

	itsLabelInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,10, 340,20);

	itsShortcutsInput =
		jnew JXInputField(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,40, 60,20);

// end Panel

	dlog->AddPanel(this, container);

	itsLabelInput->SetIsRequired();
	itsLabelInput->GetText()->SetText(label);

	itsShortcutsInput->GetText()->SetText(shortcuts);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WidgetLabelPanel::~WidgetLabelPanel()
{
}

/******************************************************************************
 Validate (virtual)

 ******************************************************************************/

bool
WidgetLabelPanel::Validate()
	const
{
	const JString& shortcuts = itsShortcutsInput->GetText()->GetText();
	if (shortcuts.Contains("#"))
	{
		bool found = false;

		JStringIterator iter(shortcuts);
		JUtf8Character c;
		while (iter.Next(&c))
		{
			if (c == '#' && iter.Next(&c) && c.IsAlnum() &&
				itsLabelInput->GetText()->GetText().Contains(c, JString::kIgnoreCase))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			JGetUserNotification()->ReportError(
				JGetString("MismatchedShortcutKey::WidgetLabelPanel"));
			JXFocusWidgetTask::Focus(itsShortcutsInput);
			return false;
		}
	}

	return true;
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
