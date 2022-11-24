/******************************************************************************
 PrintWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "PrintWidgetDir.h"
#include "PrintWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrintWidgetDir::PrintWidgetDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();

	// Create the printer object
	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );

	// We need to know when a dialog is closed
	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrintWidgetDir::~PrintWidgetDir()
{
	// We need to delete this, the window will jdelete everything else
	jdelete itsPrinter;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
PrintWidgetDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// Check if the print button was pressed
	if (sender == itsPrintButton && message.Is(JXButton::kPushed))
	{
		// Start the printing process by opening the dialog
		itsPrinter->BeginUserPrintSetup();
	}

	// This is the dialog's closing message
	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
	{
		// See if the user cancelled the print job
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			// If the user didn't cancel, print
			itsWidget->Print(*itsPrinter);
		}
	}

	else
	{
		// Pass the message up the inheritance tree
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 BuildWindow
		This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
PrintWidgetDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::PrintWidgetDir"));
	assert( window != nullptr );

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the print button
	itsPrintButton =
		jnew JXTextButton(JGetString("PrintLabel::PrintWidgetDir"), window,
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0, 0, 300, 20);

	// We need to hear when the button has been pressed
	ListenTo(itsPrintButton);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 300,180);
	assert( scrollbarSet != nullptr );

	// Create the custom widget with the scrollbarset as its enclosure
	PrintWidget* widget =
		jnew PrintWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != nullptr );

	// Fit the widget within the scrollbarset enclosure
	widget->FitToEnclosure(true, true);
	itsWidget = widget;
}
