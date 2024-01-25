/******************************************************************************
 ColorChooserPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ColorChooserPanel.h"
#include "WidgetParametersDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXChooseColorDialog.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ColorChooserPanel::ColorChooserPanel
	(
	WidgetParametersDialog* dlog,

	const JColorID color
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,60);
	assert( container != nullptr );

	auto* colorSampleLabel =
		jnew JXStaticText(JGetString("colorSampleLabel::ColorChooserPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 60,20);
	colorSampleLabel->SetToLabel(false);

	itsColorSample =
		jnew JXTextButton(JGetString("itsColorSample::ColorChooserPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,10, 40,40);

// end Panel

	dlog->AddPanel(this, container);

	itsColorSample->SetBackColor(color);

	ListenTo(itsColorSample, std::function([this](const JXButton::Pushed&)
	{
		auto* dlog = jnew JXChooseColorDialog(itsColorSample->GetBackColor());
		if (dlog->DoDialog())
		{
			itsColorSample->SetBackColor(dlog->GetColor());
		}
	}));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ColorChooserPanel::~ColorChooserPanel()
{
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
ColorChooserPanel::GetValues
	(
	JColorID* color
	)
{
	*color = itsColorSample->GetBackColor();
}
