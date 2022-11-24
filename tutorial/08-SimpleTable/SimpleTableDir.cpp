/******************************************************************************
 SimpleTableDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "SimpleTableDir.h"
#include "SimpleTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::PrintWidgetDir"));
	assert( window != nullptr );

	// Set sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set to hold the table
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( scrollbarSet != nullptr );

	// Create our SimpleTable. It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	SimpleTable* table =
		jnew SimpleTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( table != nullptr );
	table->FitToEnclosure();
}
