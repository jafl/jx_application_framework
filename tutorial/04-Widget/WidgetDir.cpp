/******************************************************************************
 WidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "WidgetDir.h"
#include "Widget.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WidgetDir::WidgetDir
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

WidgetDir::~WidgetDir()
{
	// Nothing to delete
	// Window is deleted automatically by its director
	// Widget is deleted automatically by its enclosure
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
WidgetDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::WidgetDir"));
	assert( window != nullptr );

	// Set sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create our custom widget
	jnew Widget(window, JXWidget::kHElastic, JXWidget::kVElastic, 0, 0, 300, 200);
}
