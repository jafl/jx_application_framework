/******************************************************************************
 ScrollingWidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "ScrollingWidgetDir.h"
#include "ScrollingWidget.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

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
 Destructor

 ******************************************************************************/

ScrollingWidgetDir::~ScrollingWidgetDir()
{
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
	JXWindow* window = new JXWindow(this, 300,200, "Scrolling Program");
    assert( window != NULL );

    // Give the window to the director
    SetWindow(window);

    // Set window sizing
    window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set to hold the custom widget
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( scrollbarSet != NULL );

	// Create our custom widget.  It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	ScrollingWidget* widget = 
		new ScrollingWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( widget != NULL );

	// Adjust the widget to fit into the scrollbarset
	widget->FitToEnclosure(kJTrue, kJTrue);
}
