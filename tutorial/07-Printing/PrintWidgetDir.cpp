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
	ListenTo(itsPrintButton, std::function([this](const JXButton::Pushed& msg)
	{
		// Start the printing process by opening the dialog
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			// The user didn't cancel, so print
			itsWidget->Print(*itsPrinter);
		}
	}));

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 300,180);

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
