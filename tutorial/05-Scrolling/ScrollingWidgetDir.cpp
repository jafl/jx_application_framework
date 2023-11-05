/******************************************************************************
 ScrollingWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "ScrollingWidgetDir.h"
#include "ScrollingWidget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ScrollingWidgetDir::ScrollingWidgetDir
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
ScrollingWidgetDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::ScrollingWidgetDir"));
	assert( window != nullptr );

	// Set window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set to hold the custom widget
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);

	// Create our custom widget.  It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	ScrollingWidget* widget =
		jnew ScrollingWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != nullptr );

	// Adjust the widget to fit into the scrollbarset
	widget->FitToEnclosure(true, true);
}
