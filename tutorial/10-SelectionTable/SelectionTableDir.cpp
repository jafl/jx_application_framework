/******************************************************************************
 SelectionTableDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "SelectionTableDir.h"
#include "SelectionTable.h"
#include <JXMenuBar.h>
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SelectionTableDir::SelectionTableDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	// Create the array that will hold the table's data
	itsData = new JArray<JIndex>;
	assert(itsData != NULL);

	// Append 3 elements to the array.
	itsData->AppendElement(4);
	itsData->AppendElement(1);
	itsData->AppendElement(9);

	// Set up the window and all of its contents.
	BuildWindow();

	// Add 3 more elements and change another element. If the table
	// hasn't called ListenTo(itsData), it will never hear about the
	// following additions.
	itsData->AppendElement(7);
	itsData->AppendElement(8);
	itsData->AppendElement(2);
	itsData->SetElement(5,3);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SelectionTableDir::~SelectionTableDir()
{
	delete itsData;

	// Window is deleted automatically by its director
	// SelectionTable is deleted automatically by its enclosure
}

/******************************************************************************
 BuildWindow
 	
 	This is a convenient and organized way of putting all of the initial 
 	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
SelectionTableDir::BuildWindow()
{
	// Create the window
	JXWindow* window = new JXWindow(this, 300,200, "Test SelectionTable Program");
    assert( window != NULL );
    
    // Give the window to the director
    SetWindow(window);
    
    // Set sizing
    window->SetMinSize(300,200);
	window->SetMaxSize(800,600);
	
    // Create the menu bar so that it stays on top, but expands as the window
    // expands. 
    JXMenuBar* menuBar = 
    	new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop, 
    					0,0, 300,kJXDefaultMenuBarHeight);
    assert( menuBar != NULL );
    
	// Create the scrollbar set to hold the table
	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,kJXDefaultMenuBarHeight, 300,200-kJXDefaultMenuBarHeight);
	assert( scrollbarSet != NULL );

	// Create our SelectionTable. It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	SelectionTable* table = 
		new SelectionTable(menuBar, itsData, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( table != NULL );
	table->FitToEnclosure();
}
