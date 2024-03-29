/******************************************************************************
 JXUNDialogBase.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXUNDialogBase.h"
#include "JXUserNotification.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXStaticText.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUNDialogBase::JXUNDialogBase()
	:
	JXModalDialogDirector()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUNDialogBase::~JXUNDialogBase()
{
}

/******************************************************************************
 Init (protected)

 ******************************************************************************/

void
JXUNDialogBase::Init
	(
	JXWindow*		window,
	JXStaticText*	text,
	const JString&	message
	)
{
	text->SetBorderWidth(0);
	text->SetBackgroundColor(JColorManager::GetDefaultBackColor());
	if (JUserNotification::GetBreakMessageCROnly())
	{
		text->SetBreakCROnly(true);
	}
	text->GetText()->SetText(message);

	// adjust window size to fit text - more sophisticated than ftc

	const JRect apG = text->GetApertureGlobal();
	const JRect dB  = GetDisplay()->GetBounds();

	JSize dw          = 0;
	const JSize prefw = text->TEGetMinPreferredGUIWidth();
	const JSize apw   = apG.width();
	if (prefw > apw)
	{
		dw = prefw - apw;
		text->AdjustSize(dw, 0);
	}

	JSize dh              = 0;
	const JSize aph       = apG.height();
	const JCoordinate dw1 = text->GetFrameWidth();
	while (true)
	{
		const JSize bdh = text->GetBoundsHeight();
		if (bdh > aph)
		{
			dh = bdh - aph;
		}

		if (window->GetBoundsHeight() + dh <= (JSize) dB.height())
		{
			break;
		}

		// We don't check for excessive width because excessive height is worse.
		// Excessive height means that the title bar won't be visible.

		dw += dw1;
		text->AdjustSize(dw1, 0);

		// catch the case when the height doesn't decrease

		if (text->GetBoundsHeight() == (JCoordinate) bdh)
		{
			dw -= dw1;
			text->AdjustSize(-dw1, 0);
			break;
		}
	}

	text->SetSizing(JXWidget::kFixedLeft, JXWidget::kVElastic);		// maintain width

	window->AdjustSize(dw, dh);
	window->HideFromTaskbar();

	text->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);		// allow ftc expansion
}
