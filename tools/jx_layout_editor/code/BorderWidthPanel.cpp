/******************************************************************************
 BorderWidthPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "BorderWidthPanel.h"
#include "WidgetParametersDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

BorderWidthPanel::BorderWidthPanel
	(
	WidgetParametersDialog* dlog,

	const JSize width
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	auto* borderWidthLabel =
		jnew JXStaticText(JGetString("borderWidthLabel::BorderWidthPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 90,20);
	borderWidthLabel->SetToLabel(false);

	itsBorderWidthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 50,20);
	itsBorderWidthInput->SetLowerLimit(1);

// end Panel

	dlog->AddPanel(this, container);

	itsBorderWidthInput->SetValue(width);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BorderWidthPanel::~BorderWidthPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
BorderWidthPanel::GetValues
	(
	JSize* width
	)
{
	JInteger w;
	const bool ok = itsBorderWidthInput->GetValue(&w);
	assert( ok );

	*width = w;
}
