/******************************************************************************
 WidgetDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "WidgetDir.h"
#include "Widget.h"
#include <JXWindow.h>
#include <jAssert.h>

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
	JXWindow* window = new JXWindow(this, 300,200, "Test Widget Program");
    assert( window != NULL );
    
    // Give the window to the director
    SetWindow(window);
    
    // Set sizing
    window->SetMinSize(300,200);
	window->SetMaxSize(800,600);
	
	// Create our custom widget
	Widget* widget = 
		new Widget(window, JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 300, 200);
	assert( widget != NULL );
}
