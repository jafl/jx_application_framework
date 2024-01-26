/******************************************************************************
 ComplexBorderWidthPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ComplexBorderWidthPanel.h"
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

ComplexBorderWidthPanel::ComplexBorderWidthPanel
	(
	WidgetParametersDialog* dlog,

	const JSize outside,
	const JSize between,
	const JSize inside
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,40);
	assert( container != nullptr );

	auto* outsideWidthLabel =
		jnew JXStaticText(JGetString("outsideWidthLabel::ComplexBorderWidthPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 90,20);
	outsideWidthLabel->SetToLabel(false);

	auto* betweenWidthLabel =
		jnew JXStaticText(JGetString("betweenWidthLabel::ComplexBorderWidthPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 170,10, 90,20);
	betweenWidthLabel->SetToLabel(false);

	auto* insideWidthLabel =
		jnew JXStaticText(JGetString("insideWidthLabel::ComplexBorderWidthPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 320,10, 90,20);
	insideWidthLabel->SetToLabel(false);

	itsOutsideWidthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,10, 40,20);
	itsOutsideWidthInput->SetLowerLimit(1);

	itsBetweenWidthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 260,10, 40,20);
	itsBetweenWidthInput->SetLowerLimit(0);

	itsInsideWidthInput =
		jnew JXIntegerInput(container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 410,10, 40,20);
	itsInsideWidthInput->SetLowerLimit(1);

// end Panel

	dlog->AddPanel(this, container);

	itsOutsideWidthInput->SetValue(outside);
	itsBetweenWidthInput->SetValue(between);
	itsInsideWidthInput->SetValue(inside);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ComplexBorderWidthPanel::~ComplexBorderWidthPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
ComplexBorderWidthPanel::GetValues
	(
	JSize* outside,
	JSize* between,
	JSize* inside
	)
{
	JInteger w;
	bool ok = itsOutsideWidthInput->GetValue(&w);
	assert( ok );
	*outside = w;

	ok = itsBetweenWidthInput->GetValue(&w);
	assert( ok );
	*between = w;

	ok = itsInsideWidthInput->GetValue(&w);
	assert( ok );
	*inside = w;
}
