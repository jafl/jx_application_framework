/******************************************************************************
 PrintWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "PrintWidgetDir.h"
#include "PrintWidget.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXPSPrinter.h>
#include <JXTextButton.h>
#include <jAssert.h>

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
	itsPrinter = new JXPSPrinter(GetDisplay(), (GetWindow())->GetColormap());
	assert( itsPrinter != NULL );

	// We need to know when a dialog is closed
	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrintWidgetDir::~PrintWidgetDir()
{
	// We need to delete this, the window will delete everything else
	delete itsPrinter;
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
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
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
	JXWindow* window = new JXWindow(this, 300,200, "Printing Test Program");
	assert( window != NULL );

	// Give the window to the director
	SetWindow(window);

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the print button
	itsPrintButton = 
		new JXTextButton("Print", window, 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 300, 20);

	// We need to hear when the button has been pressed
	ListenTo(itsPrintButton);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 300,180);
	assert( scrollbarSet != NULL );

	// Create the custom widget with the scrollbarset as its enclosure		
	PrintWidget* widget = 
		new PrintWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != NULL );

	// Fit the widget within the scrollbarset enclosure
	widget->FitToEnclosure(kJTrue, kJTrue);
	itsWidget = widget;
}
