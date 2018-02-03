/******************************************************************************
 JXSplashWindow.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#include <JXSplashWindow.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXEmbossedRect.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	In the versions that take JXImage*, we take ownership of the image.

 ******************************************************************************/

JXSplashWindow::JXSplashWindow
	(
	const JXPM&		imageData,
	const JString&	text,
	const time_t	displayInterval
	)
	:
	JXWindowDirector(JXGetApplication())
{
	JXDisplay* d   = (JXGetApplication())->GetCurrentDisplay();
	JXImage* image = jnew JXImage(d, imageData);
	assert( image != NULL );

	BuildWindow(image, text, displayInterval);
}

JXSplashWindow::JXSplashWindow
	(
	JXImage*		image,
	const JString&	text,
	const time_t	displayInterval
	)
	:
	JXWindowDirector(JXGetApplication())
{
	BuildWindow(image, text, displayInterval);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSplashWindow::~JXSplashWindow()
{
}

/******************************************************************************
 BuildWindow (private)

	[ 20 |image| 20 |text| 20 ]

 ******************************************************************************/

void
JXSplashWindow::BuildWindow
	(
	JXImage*		image,
	const JString&	text,
	const time_t	displayInterval
	)
{
	const JCoordinate borderWidth = 2;
	const JCoordinate totalWidth  = 400;
	const JCoordinate totalHeight = 100;
	const JCoordinate marginWidth = 20;
	const JCoordinate imageWidth  = image->GetWidth();

	JXWindow* window = jnew JXWindow(this, totalWidth + 3*borderWidth,
									totalHeight + 3*borderWidth,
									JGetString("WindowTitle::JXSplashWindow"), kJTrue);
	assert( window != NULL );

	JXEmbossedRect* border =
		jnew JXEmbossedRect(window, JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( border != NULL);
	border->FitToEnclosure();
	border->SetWidths(borderWidth, borderWidth, borderWidth);

	JXImageWidget* iconWidget =
		jnew JXImageWidget(border, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						  marginWidth, marginWidth,
						  imageWidth, image->GetHeight());
	assert( iconWidget != NULL );
	iconWidget->SetImage(image, kJTrue);

	JXStaticText* textWidget =
		jnew JXStaticText(text, border,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 2*marginWidth + imageWidth, marginWidth,
						 totalWidth - (3*marginWidth + imageWidth),
						 totalHeight - 2*marginWidth);
	assert( textWidget != NULL );

	// adjust window to fit icon

	const JCoordinate minHeight = iconWidget->GetBoundsHeight() + 2*marginWidth;
	if (minHeight > window->GetApertureHeight())
		{
		window->AdjustSize(0, minHeight - window->GetApertureHeight());
		}

	// adjust window to fit text

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}

	// close after specified time interval

	JXTimerTask* task = jnew JXTimerTask(displayInterval * 1000, kJTrue);
	assert( task != NULL );
	ListenTo(task);
	task->Start();

	// place window

	window->PlaceAsDialogWindow();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXSplashWindow::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JXTimerTask::kTimerWentOff))
		{
		Close();
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}
