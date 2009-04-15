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

#include <JXStdInc.h>
#include "SelectionTable.h"
#include <JXMenuBar.h>
#include <JXWindowPainter.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
#include <JOrderedSet.h>
#include <JString.h>
#include <JTableSelection.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jAssert.h>

// The default row height, column width, and insertion value.
const JCoordinate	kDefRowHeight	= 20;
const JCoordinate	kDefColWidth	= 80;
const JIndex 		kDefInsertValue	= 12;	

// These define the menu title and the menu items.
// The '|' separates menu items.  The complete syntax
// is described in JXTextMenuData.doc in SetMenuItems()
// and ParseMenuItemStr().

static const JCharacter* kTableMenuTitleStr = "Table";
static const JCharacter* kTableMenuStr = 
	"Insert %k Meta-I | Remove | Quit %k Meta-Q";
	
enum
{
	kInsertCmd = 1,
	kRemoveCmd,
	kQuitCmd
};

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

	// We need to set this to NULL in case we receive a message before
	// we create it.
	itsTableMenu = NULL;

	// This data needs only one column
	AppendCols(1, kDefColWidth);

	// We need to add a row for each element in the data array
	AppendRows(itsData->GetElementCount(), kDefRowHeight);

	// The table is now in sync with the data array, but in 
	// order to hear about changes in the data, we have to listen
	// for messages from the data.
	ListenTo(itsData);

    // Attach our menu to the menu bar.
    itsTableMenu = menuBar->AppendTextMenu(kTableMenuTitleStr);
    
    // Set the menu items in our menu.
    itsTableMenu->SetMenuItems(kTableMenuStr);
    
    // Set the menu to never disable the menu items.
    itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);
    
    // The table needs to listen to the menu for messages. 
	ListenTo(itsTableMenu);
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
	JString cellNumber(itsData->GetElement(cell.y));

	// Draw the JString that holds the value. 
	p.String(rect, cellNumber, JPainter::kHAlignLeft, JPainter::kVAlignTop);
}

/******************************************************************************
 Receive

	We listen to the data array for changes.

 ******************************************************************************/

void
SelectionTable::Receive
	(
	JBroadcaster* 	sender, 
	const Message& 	message
	)
{
	// Here we check to see what messages we have received.

	// We first check if the sender is our data array
	if (sender == itsData)
		{
		// Our data array sent us a message

		// Was data inserted?
		if (message.Is(JOrderedSetT::kElementsInserted))
			{
			// cast the message to an ElementsInserted object
			const JOrderedSetT::ElementsInserted* info = 
				dynamic_cast(const JOrderedSetT::ElementsInserted*, &message);
			assert(info != NULL);

			// For each element inserted, we insert a row
			InsertRows(info->GetFirstIndex(), info->GetCount(), kDefRowHeight);
			}

		// Was data removed?
		else if (message.Is(JOrderedSetT::kElementsRemoved))
			{
			// cast the message to an ElementsRemoved object
			const JOrderedSetT::ElementsRemoved* info = 
				dynamic_cast(const JOrderedSetT::ElementsRemoved*, &message);
			assert(info != NULL);

			// Remove corresponding table rows.
			for (JSize i = info->GetLastIndex(); i >= info->GetFirstIndex() ; i--)
				{
				RemoveNextRows(info->GetFirstIndex(), info->GetCount());
				}
			}

		// Was an element changed?
		else if (message.Is(JOrderedSetT::kElementChanged))
			{
			// cast the message to an ElementsRemoved object
			const JOrderedSetT::ElementChanged* info = 
				dynamic_cast(const JOrderedSetT::ElementChanged*, &message);
			assert(info != NULL);

			// The element changed, so redraw it.
			// (This would not be necessary if we were using a
			//  class derived from JTableData.)
			TableRefreshRow(info->GetFirstIndex());
			}
		}

	// Did the Table menu send a message?
	else if (sender == itsTableMenu)
		{
		// Does the menu need an update?
		if (message.Is(JXMenu::kNeedsUpdate))
			{
			UpdateTableMenu();
			}

		// Has a menu item been selected?
		else if (message.Is(JXMenu::kItemSelected))
			{
			// cast the message to an ItemSelecte object.
			// This will tell us which item was selected.
			const JXMenu::ItemSelected* info = 
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert(info != NULL);

			// Pass the selected menu item to our menu handler function.
			HandleTableMenu(info->GetIndex());
			}
		}

	// pass the message to our base class
	else
		{
		JXTable::Receive(sender, message);
		}
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
		const JBoolean ok = selection.GetFirstSelectedCell(&cell);
		assert(ok);

		// The default value is inserted before the selected cell.
		itsData->InsertElementAtIndex(cell.y, kDefInsertValue);
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
			itsData->RemoveElement(cell.y);
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
