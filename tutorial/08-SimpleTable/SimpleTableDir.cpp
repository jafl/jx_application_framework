/******************************************************************************
 SimpleTableDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "SimpleTableDir.h"
#include "SimpleTable.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SimpleTableDir::SimpleTableDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	// Set up the window and all of its contents.
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SimpleTableDir::~SimpleTableDir()
{
	// Nothing to delete
	// Window is deleted automatically by its director
	// SimpleTable is deleted automatically by its enclosure
}

/******************************************************************************
 BuildWindow
 	
 	This is a convenient and organized way of putting all of the initial 
 	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
SimpleTableDir::BuildWindow()
{
	// Create the window
	JXWindow* window = new JXWindow(this, 300,200, "Test SimpleTable Program");
    assert( window != NULL );
    
    // Give the window to the director
    SetWindow(window);
    
    // Set sizing
    window->SetMinSize(300,200);
	window->SetMaxSize(800,600);
	
	// Create the scrollbar set to hold the table
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( scrollbarSet != NULL );

	// Create our SimpleTable. It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	SimpleTable* table = 
		new SimpleTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( table != NULL );
	table->FitToEnclosure();
}
