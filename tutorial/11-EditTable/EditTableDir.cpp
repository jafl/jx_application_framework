/******************************************************************************
 EditTableDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "EditTableDir.h"
#include "EditTable.h"
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditTableDir::EditTableDir
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
	itsData->AppendItem(4);
	itsData->AppendItem(1);
	itsData->AppendItem(9);

	// Set up the window and all of its contents.
	BuildWindow();

	// Add 3 more elements and change another element. If the table
	// hasn't called ListenTo(itsData), it will never hear about the
	// following additions.
	itsData->AppendItem(7);
	itsData->AppendItem(8);
	itsData->AppendItem(2);
	itsData->SetItem(5,3);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditTableDir::~EditTableDir()
{
	jdelete itsData;

	// Window is deleted automatically by its director
	// EditTable is deleted automatically by its enclosure
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
EditTableDir::BuildWindow()
{
	// Create the window
	JXWindow* window = jnew JXWindow(this, 300,200, JGetString("WindowTitle::EditTableDir"));
	assert( window != nullptr );

	// Set sizing
	window->SetMinSize(300,200);
	window->SetMaxSize(800,600);

	// Create the menu bar so that it stays on top, but expands as the window
	// expands.
	JXMenuBar* menuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, 300,kJXDefaultMenuBarHeight);

	// Create the scrollbar set to hold the table
	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight, 300,200-kJXDefaultMenuBarHeight);

	// Create our EditTable. It must be placed inside the
	// special widget that JXScrollbarSet creates.  We get a
	// pointer to this special widget by calling GetScrollEnclosure().
	EditTable* table =
		jnew EditTable(menuBar, itsData,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( table != nullptr );
	table->FitToEnclosure();
}
