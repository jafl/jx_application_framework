/******************************************************************************
 DragWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "DragWidgetDir.h"
#include "DragWidget.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

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
 Destructor

 ******************************************************************************/

DragWidgetDir::~DragWidgetDir()
{
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
	JXWindow* window = new JXWindow(this, 300,200, "Drag Painter Program");
	assert( window != NULL );

	// Set the window sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( scrollbarSet != NULL );

	// Create the custom widget with the scrollbarset as its enclosure
	DragWidget* widget =
		new DragWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != NULL );

	// Fit the widget within the scrollbarset enclosure
	widget->FitToEnclosure(kJTrue, kJTrue);
}
