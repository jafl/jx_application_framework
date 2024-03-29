/******************************************************************************
 SelectionTable.cpp

	The table is extended to allow selection. If the user clicks in a cell
	that cell is selected and hilighted. If the user clicks outside of all
	the cells but still within the table, all of the cells are unselected.
	A menu bar is added, so insertions can be made before the selected cell,
	and removals can be made of selected cells.

	BASE CLASS = JXTable

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "SelectionTable.h"
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

// The default row height, column width, and insertion value.
const JCoordinate	kDefRowHeight	= 20;
const JCoordinate	kDefColWidth	= 80;
const JIndex 		kDefInsertValue	= 12;

#include "SelectionTable-Table.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

SelectionTable::SelectionTable
	(
	JXMenuBar* 			menuBar,
	JArray<JIndex>* 	data,
	JXScrollbarSet* 	scrollbarSet,
	JXContainer* 		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate 	x,
	const JCoordinate 	y,
	const JCoordinate 	w,
	const JCoordinate 	h
	)
	:
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	// This will be our data, but we don't own it so we can't delete it.
	itsData = data;

	// We need to set this to nullptr in case we receive a message before
	// we create it.
	itsTableMenu = nullptr;

	// This data needs only one column
	AppendCols(1, kDefColWidth);

	// We need to add a row for each element in the data array
	AppendRows(itsData->GetItemCount(), kDefRowHeight);

	// The table is now in sync with the data array, but in
	// order to hear about changes in the data, we have to listen
	// for messages from the data.
	ListenTo(itsData, std::function([this](const JListT::ItemsInserted& msg)
	{
		// For each element inserted, we insert a row
		InsertRows(msg.GetFirstIndex(), msg.GetCount(), kDefRowHeight);
	}));

	ListenTo(itsData, std::function([this](const JListT::ItemsRemoved& msg)
	{
		// Remove the corresponding table rows. 
		RemoveNextRows(msg.GetFirstIndex(), msg.GetCount());
	}));

	ListenTo(itsData, std::function([this](const JListT::ItemsChanged& msg)
	{
		// The element changed, so redraw it.
		// (This would not be necessary if we were using a
		//  class derived from JTableData.)
		for (JIndex i=msg.GetFirstIndex(); i<=msg.GetLastIndex(); i++)
		{
			TableRefreshRow(i);
		}
	}));

	// Attach our menu to the menu bar.
	itsTableMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::SelectionTable"));

	// Set the menu items in our menu.
	itsTableMenu->SetMenuItems(kTableMenuStr);

	// Set the menu to never disable the menu items.
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);

	// The table needs to listen to the menu for messages.
	itsTableMenu->AttachHandlers(this,
		&SelectionTable::UpdateTableMenu,
		&SelectionTable::HandleTableMenu);

	// Apply configuration defined in the menu editor.
	ConfigureTableMenu(itsTableMenu);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SelectionTable::~SelectionTable()
{
	// we don't own itsData, so we don't delete it.
	// itsTableMenu is deleted automatically by its enclosure.
}

/******************************************************************************
 TableDrawCell

	This gets called by JTable every time the table needs to
	be redrawn.

	p is the Painter that you use to draw to the screen.  The main reason
	for Painter is to hide the system dependent details of drawing,
	and to provide a uniform interface for drawing to the screen, to an
	offscreen image, and to a printer.

	rect is the boundary of the cell.  The clipping rectangle has been set
	set to this so anything that you draw outside this rectangle will not
	be displayed.

	cell gives the row and column of the cell that needs to be redrawn.

	cell.x = column
	cell.y = row

 ******************************************************************************/

void
SelectionTable::TableDrawCell
	(
	JPainter& 		p,
	const JPoint& 	cell,
	const JRect& 	rect
	)
{
	// JTable keeps track of what's selected. If we want just
	// basic hilighting of the selected cells, we can have JTable
	// do it with this call:
	HilightIfSelected(p, cell, rect);

	// Convert the array's current element into a JString.
	JString cellNumber(itsData->GetItem(cell.y));

	// Draw the JString that holds the value.
	p.String(rect, cellNumber, JPainter::HAlign::kLeft, JPainter::VAlign::kTop);
}

/******************************************************************************
 HandleMouseDown

	Select a single cell or deselect all cells.

 ******************************************************************************/

void
SelectionTable::HandleMouseDown
	(
	const JPoint& 			pt,
	const JXMouseButton 	button,
	const JSize 			clickCount,
	const JXButtonStates& 	buttonStates,
	const JXKeyModifiers& 	modifiers
	)
{
	if (button == kJXLeftButton)
	{
		// We first need to find out which cell contains this point.
		JPoint cell;
		if (GetCell(pt, &cell))
		{
			// We clicked in the table.

			// Get the selection object that JTable owns.
			JTableSelection& selection = GetTableSelection();

			// Select the cell that the user clicked on.
			selection.SelectCell(cell);
		}
		else
		{
			// We clicked the mouse outside of the table.

			// Get the selection object that JTable owns.
			JTableSelection& selection = GetTableSelection();

			// Clear the selection.
			selection.ClearSelection();
		}
	}

	// Let the base class handle the wheel mouse.
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 UpdateTableMenu

	Before the menu is displayed, we update the menu entries.

 ******************************************************************************/

void
SelectionTable::UpdateTableMenu()
{
	// Get the selection object that JTable owns.
	JTableSelection& selection = GetTableSelection();

	// Check that only one cell is selected.
	if (selection.GetSelectedCellCount() != 1)
	{
		// Too many cells are selected, or none are, so disallow insertion
		itsTableMenu->DisableItem(kInsertCmd);
	}
	else
	{
		// Only one cell is selected, so allow insertion
		itsTableMenu->EnableItem(kInsertCmd);
	}

	itsTableMenu->SetItemEnabled(kRemoveCmd, selection.HasSelection());
}

/******************************************************************************
 HandleTableMenu

	Respond to menu selections.

 ******************************************************************************/

void
SelectionTable::HandleTableMenu
	(
	const JIndex index
	)
{
	// A menu item from the Table menu was selected.

	// Was it the Insert command?
	if (index == kInsertCmd)
	{
		// Get the selection object that JTable owns.
		JTableSelection& selection = GetTableSelection();

		// Since we disable the Insert command if there is more than
		// one cell selected or if none are selected, we know that
		// one cell is selected. We assert that only one cell is
		// selected.
		JPoint cell;

		// This finds the first selected cell, which in our case is
		// the only selected cell.
		const bool ok = selection.GetFirstSelectedCell(&cell);
		assert(ok);

		// The default value is inserted before the selected cell.
		itsData->InsertItemAtIndex(cell.y, kDefInsertValue);
	}

	// Was it the Remove command?
	else if (index == kRemoveCmd)
	{
		// Get the selection object that JTable owns.
		JTableSelection& selection = GetTableSelection();

		// Create an iterator to iterate over each selected item.
		JTableSelectionIterator iter(&selection);

		// Loop through each selected cell.
		JPoint cell;
		while (iter.Next(&cell))
		{

			// Remove the element corresponding to the cell selected.
			// The table will automatically adjust itself in the
			// Receive function.
			itsData->RemoveItem(cell.y);
		}
	}

	// Was it the Quit command?
	else if (index == kQuitCmd)
	{
		// Get the application object (from jXGlobals.h) and call Quit
		// to exit the program.
		JXGetApplication()->Quit();
	}
}
