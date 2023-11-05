/******************************************************************************
 DragWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "DragWidgetDir.h"
#include "DragWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DragWidgetDir::DragWidgetDir
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
DragWidgetDir::BuildWindow()
{
		// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::DragWidgetDir"));
	assert( window != nullptr );

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);

	// Create the custom widget with the scrollbarset as its enclosure
	DragWidget* widget =
		jnew DragWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != nullptr );

	// Fit the widget within the scrollbarset enclosure
	widget->FitToEnclosure(true, true);
}
