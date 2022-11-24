/******************************************************************************
 DataTableDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "DataTableDir.h"
#include "DataTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DataTableDir::DataTableDir
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	// Create the array that will hold the table's data
	itsData = jnew JArray<JIndex>;
	assert(itsData != nullptr);

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

DataTableDir::~DataTableDir()
{
	jdelete itsData;

	// Window is deleted automatically by its director
	// DataTable is deleted automatically by its enclosure
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
DataTableDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::DataTableDir"));
	assert( window != nullptr );

	// Set sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the scrollbar set to hold the table
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( scrollbarSet != nullptr );

	// Create our DataTable. It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	DataTable* table =
		jnew DataTable(itsData, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( table != nullptr );
	table->FitToEnclosure();
}
