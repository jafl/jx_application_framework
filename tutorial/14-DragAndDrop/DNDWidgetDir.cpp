/******************************************************************************
 DNDWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 2000.

 ******************************************************************************/

#include "DNDWidgetDir.h"
#include "DNDWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DNDWidgetDir::DNDWidgetDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
DNDWidgetDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::DNDWidgetDir"));

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);

	// Create the custom widget with the scrollbarset as its enclosure
	DNDWidget* widget =
		jnew DNDWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);

	// Fit the widget within the scrollbarset enclosure
	widget->FitToEnclosure(true, true);
}
