/******************************************************************************
 EditTable.cpp

	We now extend the table program to allow in-cell editing. As before,
	you can select a cell, but now only with a single click. If the user
	double clicks on a cell, he will then be able to edit the cell's
	contents. When editing is finished, the return key will accept the
	change (or refuse it if the value is invalid) and begin editing on the
	next cell. To cancel editing, the user hits the escape key.

	BASE CLASS = JXEditTable

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "EditTable.h"

#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXIntegerInput.h>
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

// The margins -- a bit of space between the edge of the cell and the text.
const JCoordinate kHMargin = JTextEditor::kMinLeftMarginWidth + 1;
const JCoordinate kVMargin = 1;

// This defines the menu items.
// The '|' separates menu items.  The complete syntax
// is described in JXTextMenuData.doc in SetMenuItems()
// and ParseMenuItemStr().

static const JUtf8Byte* kTableMenuStr =
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

EditTable::EditTable
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
	JXEditTable(kDefRowHeight, kDefColWidth, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
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
	itsTableMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::EditTable"));

	// Set the menu items in our menu.
	itsTableMenu->SetMenuItems(kTableMenuStr);

	// Set the menu to never disable the menu items.
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);

	// The table needs to listen to the menu for messages.
	itsTableMenu->AttachHandlers(this,
		&EditTable::UpdateTableMenu,
		&EditTable::HandleTableMenu);

	// This is nullptr, because we want to make sure we only create one at
	// a time.
	itsIntegerInputField = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditTable::~EditTable()
{
	// we don't own itsData, so we don't delete it.
	// itsTableMenu is deleted automatically by its enclosure.
	// itsIntegerInputField is deleted automatically by its enclosure.
}

/******************************************************************************
 TableDrawCell

	This gets called by JTable every time the cell passed to
	the function needs to be redrawn.

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
EditTable::TableDrawCell
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
	JString cellNumber((JUInt64) itsData->GetItem(cell.y));

	// Apply the margins.
	JRect r = rect;
	r.Shrink(kHMargin, kVMargin);

	// Draw the JString that holds the value.
	p.String(r, cellNumber, JPainter::HAlign::kLeft, JPainter::VAlign::kTop);
}

/******************************************************************************
 HandleMouseDown

	The mouse has been clicked in the table. It may or may not have been in
	cell.

 ******************************************************************************/

void
EditTable::HandleMouseDown
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

			// select on single click, edit on double click
			if (clickCount == 2)
			{
				// Get the selection object that JTable owns.
				JTableSelection& selection = GetTableSelection();

				// Clear the selection
				selection.ClearSelection();

				// Start editing
				BeginEditing(cell, true);
			}
			else
			{
				// Get the selection object that JTable owns.
				JTableSelection& selection = GetTableSelection();

				// Select the cell that the user clicked on.
				selection.SelectCell(cell);
			}
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
EditTable::UpdateTableMenu()
{
	// Get the selection object that JTable owns.
	JTableSelection& selection = GetTableSelection();

	// Check that only one cell is selected.
	itsTableMenu->SetItemEnabled(kInsertCmd, selection.GetSelectedCellCount() == 1);

	itsTableMenu->SetItemEnabled(kRemoveCmd, selection.GetSelectedCellCount() > 0);
}

/******************************************************************************
 HandleTableMenu

	Respond to menu selections.

 ******************************************************************************/

void
EditTable::HandleTableMenu
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

		// Create an iterator to iterate over each selected item.
		JTableSelectionIterator iter(&selection);

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

/******************************************************************************
 CreateXInputField (virtual protected)

	Create a widget to handle the input in the specified cell.

	x,y,w,h can be passed directly to the widget constructor.

 ******************************************************************************/

JXInputField*
EditTable::CreateXInputField
	(
	const JPoint&    		cell,
	const JCoordinate       x,
	const JCoordinate       y,
	const JCoordinate       w,
	const JCoordinate       h
	)
{
	// We are not supposed to have more than one input field.
	assert( itsIntegerInputField == nullptr );

	// Create the input field
	itsIntegerInputField = jnew JXIntegerInput(this, kFixedLeft, kFixedTop, x,y,w,h);

	// Set the input field's value to that of the cell.
	itsIntegerInputField->SetValue(itsData->GetItem(cell.y));

	// Set the minimum value to be zero
	itsIntegerInputField->SetLowerLimit(0);

	// Tell JXEditTable what our input field is.
	return itsIntegerInputField;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it, and
	delete the input field if successful.

	Returns true if the data is valid and the process succeeded.

 ******************************************************************************/

bool
EditTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	// This better exist if this is called!
	assert( itsIntegerInputField != nullptr );

	// If the data is invalid, return false.
	// This tells JTable to not stop editing.
	if (!itsIntegerInputField->InputValid())
	{
		return false;
	}

	// Get the value from the input field
	JInteger number;
	const bool ok = itsIntegerInputField->GetValue(&number);

	// InputValid() displays an error message if there is a
	// problem.  Since we got past that, GetValue() had better
	// return true.
	assert( ok );

	// The data must be good if we have gotten this far, so set the
	// value in the data array to the value extracted from the
	// input field.
	itsData->SetItem(cell.y, number);

	// The edit was successful, so the input field can be deleted.
	return true;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
EditTable::PrepareDeleteXInputField()
{
	// Set the input field to nullptr after an edit so we know that we have no
	// currently allocated input field.

	itsIntegerInputField = nullptr;
}
