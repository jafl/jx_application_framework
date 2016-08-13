/******************************************************************************
 JXUNDialogBase.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXUNDialogBase.h>
#include <JXUserNotification.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUNDialogBase::JXUNDialogBase
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJFalse)
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
	JXWindow*			window,
	JXStaticText*		text,
	const JCharacter*	message,
	JXImageWidget*		icon,
	const JXPM&			xpm
	)
{
	if (JUserNotification::GetBreakMessageCROnly())
		{
		text->SetBreakCROnly(kJTrue);
		}
	text->SetText(message);

	// adjust window size to fit text

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
	while (1)
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
	window->LockCurrentSize();
	window->PlaceAsDialogWindow();
	window->HideFromTaskbar();

	// display the icon

	JXImage* image = new JXImage(GetDisplay(), xpm);
	assert( image != NULL );
	image->SetDefaultState(JXImage::kRemoteStorage);
	image->ConvertToDefaultState();
	icon->SetImage(image, kJTrue);
}
