/******************************************************************************
 MenuTable.cpp

	BASE CLASS = JXEditTable, JPrefObject

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "MenuTable.h"
#include "MenuItemSelection.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kTypeColumn = 1,
	kIconColumn,
	kTextColumn,
	kShortcutColumn,
	kWindowsKeyColumn,
	kIDColumn,
	kSeparatorColumn
};

const JCoordinate kInitColWidth[] =
{
	40, 40, 100, 80, 40, 100, 40
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// Table menu

static const JUtf8Byte* kTableMenuStr =
	"  New item                   %k Meta-Shift-N %i" kNewItemAction
	"| Duplicate selected item(s)                 %i" kDuplicateItemsAction;

enum
{
	kAppendCmd = 1,
	kDuplicateCmd
};

// Type menu

static const JUtf8Byte* kTypeMenuStr =
	"  Plain %r"
	"| Checkbox %r"
	"| Radio button %r";

enum
{
	kPlainCmd = 1,
	kCheckboxCmd,
	kRadioCmd
};

// data versions

const JFileVersion kCurrentDataVersion = 0;

const JFileVersion kCurrentPrefsVersion = 0;

const JFileVersion kCurrentGeometryDataVersion = 0;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

MenuTable::MenuTable
	(
	JXMenuBar*			menuBar,
	JXTEBase*			editMenuProvider,
	JXTextMenu*			editMenu,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	JPrefObject(GetPrefsManager(), kMenuTableID),
	itsEditMenuProvider(editMenuProvider),
	itsEditMenu(editMenu),
	itsTextInput(nullptr),
	itsCharInput(nullptr)
{
	itsDNDRowIndex = 0;

	itsMenuItemXAtom =
		GetDisplay()->RegisterXAtom(MenuItemSelection::GetMenuItemXAtomName());

	const JSize rowHeight = 2*kVMarginWidth +
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	SetDefaultRowHeight(rowHeight);

	// menus

	itsTableMenu = menuBar->AppendTextMenu(JGetString("TableMenuTitle::MenuTable"));
	itsTableMenu->SetMenuItems(kTableMenuStr, "MenuTable");
	itsTableMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTableMenu->AttachHandler(this, &MenuTable::HandleTableMenu);

	menuBar->InsertMenuAfter(itsEditMenu, itsTableMenu);

	itsTypeMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	itsTypeMenu->SetMenuItems(kTypeMenuStr, "MenuTable");
	itsTypeMenu->SetToHiddenPopupMenu();
	itsTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTypeMenu->AttachHandlers(this,
		&MenuTable::UpdateTypeMenu,
		&MenuTable::HandleTypeMenu);

	itsIconMenu = jnew JXImageMenu(JString::empty, 5, this, kFixedLeft, kFixedTop, 0,0, 10,10);
//	BuildIconMenu();
	itsIconMenu->SetToHiddenPopupMenu();
	itsIconMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsIconMenu->AttachHandler(this, &MenuTable::HandleIconMenu);

	ListenTo(itsEditMenu);
	ListenTo(itsIconMenu);

	// data

	itsItemList = jnew ItemList;

	for (const auto w : kInitColWidth)
	{
		AppendCols(1, w);
	}

	JPrefObject::ReadPrefs();

	AddItem();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MenuTable::~MenuTable()
{
	itsItemList->DeleteAll();
	jdelete itsItemList;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
MenuTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXEditTable::Draw(p, rect);

	if (itsDNDRowIndex > 0)
	{
		p.ResetClipRect();

		const JSize origLineWidth = p.GetLineWidth();
		p.SetLineWidth(3);

		const JRect b = GetBounds();
		if (RowIndexValid(itsDNDRowIndex))
		{
			const JRect r = GetCellRect(JPoint(1, itsDNDRowIndex));
			p.Line(b.left, r.top, b.right, r.top);
		}
		else
		{
			const JRect r = GetCellRect(JPoint(1, GetRowCount()));
			p.Line(b.left, r.bottom, b.right, r.bottom);
		}

		p.SetLineWidth(origLineWidth);
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
MenuTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	HilightIfSelected(p, cell, rect);

	const ItemInfo info = itsItemList->GetItem(cell.y);
	if (info.separator)
	{
		JPoint pt1 = rect.bottomLeft(), pt2 = rect.bottomRight();
		pt1.y--;
		pt2.y--;
		p.Line(pt1, pt2);
	}

	if (cell.x == kTypeColumn)
	{
		const JPoint pt = rect.center();
		JRect r(pt.y, pt.x, pt.y, pt.x);
		if (info.type == JXMenu::kCheckboxType)
		{
			r.Expand(5, 5);
			JXDrawUpFrame(p, r, kJXDefaultBorderWidth, true, JColorManager::GetDefaultBackColor());
		}
		else if (info.type == JXMenu::kRadioType)
		{
			r.Expand(6, 6);
			JXDrawUpDiamond(p, r, kJXDefaultBorderWidth, true, JColorManager::GetDefaultBackColor());
		}
	}
	else if (cell.x == kIconColumn)
	{
	}
	else if (cell.x == kSeparatorColumn)
	{
		const JPoint pt = rect.center();
		JRect r(pt.y, pt.x, pt.y, pt.x);
		if (info.separator)
		{
			r.Expand(3, 3);
			p.SetFilling(true);
			p.Ellipse(r);
		}		
	}
	else if (cell.x == kWindowsKeyColumn)
	{
		p.String(rect, info.windowsKey, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
	}
	else
	{
		const JString* s = nullptr;
		if (cell.x == kTextColumn)
		{
			s = info.text;
		}
		else if (cell.x == kShortcutColumn)
		{
			s = info.shortcut;
		}
		else if (cell.x == kIDColumn)
		{
			s = info.id;
		}
		assert( s != nullptr );

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *s, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
MenuTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPt = pt;

	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
	{
		return;
	}

	if (cell.x == kTypeColumn)
	{
		SelectSingleCell(cell);
		itsTypeMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (cell.x == kIconColumn)
	{
		SelectSingleCell(cell);
		itsIconMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (cell.x == kSeparatorColumn)
	{
		SelectSingleCell(cell);
		ItemInfo info  = itsItemList->GetItem(cell.y);
		info.separator = ! info.separator;
		itsItemList->SetItem(cell.y, info);
		TableRefreshCell(cell);
	}
	else if (button == kJXLeftButton)
	{
		SelectSingleCell(cell);
		if (clickCount == 2)
		{
			BeginEditing(cell);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
MenuTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (JMouseMoved(itsStartPt, pt))
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell) && GetCellRect(cell).Contains(itsStartPt))
		{
			auto* data =
				jnew MenuItemSelection(GetDisplay(), this,
									   itsItemList->GetItem(cell.y));

			BeginDND(pt, buttonStates, modifiers, data);
		}
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
MenuTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const bool meta =
		modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex));

	if ((target == this && !meta) ||
		(target != this &&  meta))
	{
		return GetDNDManager()->GetDNDActionMoveXAtom();
	}
	else
	{
		return GetDNDManager()->GetDNDActionCopyXAtom();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept any drops from ourselves and Command from anybody else.

 ******************************************************************************/

bool
MenuTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
	{
		return true;
	}
	else if (source == nullptr)
	{
		return false;
	}

	for (auto type : typeList)
	{
		if (type == itsMenuItemXAtom)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
MenuTable::HandleDNDEnter()
{
	itsDNDRowIndex = 0;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
MenuTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	JIndex newRowIndex = itsDNDRowIndex;

	JPoint cell;
	if (GetCell(JPinInRect(pt, GetBounds()), &cell))
	{
		const JRect r = GetCellRect(cell);
		if (pt.y <= r.ycenter())
		{
			newRowIndex = cell.y;
		}
		else
		{
			newRowIndex = cell.y + 1;
		}
	}

	if (newRowIndex != itsDNDRowIndex)
	{
		itsDNDRowIndex = newRowIndex;
		Refresh();
	}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
MenuTable::HandleDNDLeave()
{
	itsDNDRowIndex = 0;
	Refresh();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept Command, we don't bother to check typeList.

 ******************************************************************************/

void
MenuTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();
	const Atom selName         = dndMgr->GetDNDSelectionName();

	if (source == this && action == dndMgr->GetDNDActionMoveXAtom())
	{
		JPoint cell;
		if (GetTableSelection().GetSingleSelectedCell(&cell) &&
			itsDNDRowIndex != JIndex(cell.y) && itsDNDRowIndex != JIndex(cell.y)+1)
		{
			JIndex newIndex = itsDNDRowIndex;
			if (newIndex > JIndex(cell.y))
			{
				newIndex--;
			}
			newIndex = JMin(newIndex, GetRowCount());

			itsItemList->MoveItemToIndex(cell.y, newIndex);
			MoveRow(cell.y, newIndex);
			SelectSingleCell(JPoint(1, newIndex));
		}
	}
	else if (source == this)
	{
		JPoint cell;
		if (GetTableSelection().GetSingleSelectedCell(&cell))
		{
			itsItemList->InsertItemAtIndex(
				itsDNDRowIndex, itsItemList->GetItem(cell.y).Copy());
			InsertRows(itsDNDRowIndex, 1);
			SelectSingleCell(JPoint(1, itsDNDRowIndex));
		}
	}
	else
	{
		Atom returnType;
		unsigned char* data;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (selMgr->GetData(selName, time, itsMenuItemXAtom,
							&returnType, &data, &dataLength, &delMethod))
		{
			if (returnType == itsMenuItemXAtom)
			{
				const std::string s((char*) data, dataLength);
				std::istringstream input(s);

				ItemInfo info = ReadMenuItem(input, kCurrentDataVersion);
				if (!input.fail())
				{
					const JIndex newIndex = JMax((JIndex) 1, itsDNDRowIndex);
					itsItemList->InsertItemAtIndex(newIndex, info);
					InsertRows(newIndex, 1);
					SelectSingleCell(JPoint(1, newIndex));

					if (action == dndMgr->GetDNDActionMoveXAtom())
					{
						selMgr->SendDeleteRequest(selName, time);
					}
				}
			}

			selMgr->DeleteData(&data, delMethod);
		}
	}

	HandleDNDLeave();
}

/******************************************************************************
 HandleKeyPress (virtual)

	Backspace/Delete:  clear menu text, menu shortcut, name

 ******************************************************************************/

void
MenuTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	bool cleared = false;
	if (c == kJDeleteKey || c == kJForwardDeleteKey)
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell))
		{
			ItemInfo info = itsItemList->GetItem(cell.y);
			if (cell.x == kTextColumn)
			{
				info.text->Clear();
				cleared = true;
			}
			else if (cell.x == kShortcutColumn)
			{
				info.shortcut->Clear();
				cleared = true;
			}
			else if (cell.x == kWindowsKeyColumn)
			{
				info.windowsKey = ' ';
				itsItemList->SetItem(cell.y, info);
				cleared = true;
			}
			else if (cell.x == kIDColumn)
			{
				info.id->Clear();
				cleared = true;
			}
		}
	}

	if (cleared)
	{
		Refresh();
	}
	else if (IsEditing())
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
	else
	{
		HandleSelectionKeyPress(c, modifiers);
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
MenuTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return (cell.x == kTextColumn || cell.x == kShortcutColumn ||
			cell.x == kWindowsKeyColumn || cell.x == kIDColumn);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
MenuTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr && itsCharInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	const ItemInfo info = itsItemList->GetItem(cell.y);

	if (cell.x == kWindowsKeyColumn)
	{
		itsCharInput = jnew JXCharInput(this, kFixedLeft, kFixedTop, x,y, w,h);
		itsCharInput->SetCharacter(info.windowsKey);
		itsCharInput->ShareEditMenu(itsEditMenu);
		return itsCharInput;
	}

	itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	const JString* text = nullptr;
	if (cell.x == kTextColumn)
	{
		text = info.text;
	}
	else if (cell.x == kShortcutColumn)
	{
		text = info.shortcut;
	}
	else if (cell.x == kIDColumn)
	{
		text = info.id;
	}
	assert( text != nullptr );

	itsTextInput->GetText()->SetText(*text);
	itsTextInput->ShareEditMenu(itsEditMenu);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
MenuTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	ItemInfo info = itsItemList->GetItem(cell.y);

	if (cell.x == kWindowsKeyColumn)
	{
		assert( itsCharInput != nullptr );
		info.windowsKey = itsCharInput->GetCharacter();
		itsItemList->SetItem(cell.y, info);
		return true;
	}

	assert( itsTextInput != nullptr );

	JString* s = nullptr;
	if (cell.x == kTextColumn)
	{
		s = info.text;
	}
	else if (cell.x == kShortcutColumn)
	{
		s = info.shortcut;
	}
	else if (cell.x == kIDColumn)
	{
		s = info.id;
	}
	assert( s != nullptr );

	if (itsTextInput->InputValid())
	{
		*s = itsTextInput->GetText()->GetText();
		s->TrimWhitespace();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
MenuTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
	itsCharInput = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
MenuTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		if (HasFocus())
		{
			UpdateEditMenu();
		}
	}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
	{
		if (HasFocus())
		{
			const auto* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleEditMenu(selection->GetIndex());
		}
	}

	else
	{
		JXEditTable::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
MenuTable::UpdateEditMenu()
{
	JIndex i;
	bool ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kUndoCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kRedoCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCutCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kPasteCmd, &i);
	assert( ok );
	itsEditMenu->DisableItem(i);

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kDeleteSelCmd, &i);
	assert( ok );
	itsEditMenu->SetItemEnabled(i, GetTableSelection().HasSelection());

	ok = itsEditMenuProvider->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &i);
	assert( ok );
	itsEditMenu->SetItemEnabled(i, GetRowCount() > 0);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
MenuTable::HandleEditMenu
	(
	const JIndex index
	)
{
	JTextEditor::CmdIndex cmd;
	if (!itsEditMenuProvider->EditMenuIndexToCmd(index, &cmd))
	{
		return;
	}

	if (cmd == JTextEditor::kDeleteSelCmd)
	{
		CancelEditing();

		JTableSelectionIterator iter(&GetTableSelection());
		JPoint cell;
		while (iter.Next(&cell))
		{
			ItemInfo info = itsItemList->GetItem(cell.y);
			info.Free();

			itsItemList->RemoveItem(cell.y);
			RemoveRow(cell.y);
		}
	}

	else if (cmd == JTextEditor::kSelectAllCmd)
	{
		GetTableSelection().SelectAll();
	}
}

/******************************************************************************
 HandleTableMenu (private)

 ******************************************************************************/

void
MenuTable::HandleTableMenu
	(
	const JIndex index
	)
{
	if (index == kAppendCmd)
	{
		AddItem();
	}
	else if (index == kDuplicateCmd)
	{
		DuplicateSelectedItems();
	}
}

/******************************************************************************
 AddItem (private)

 ******************************************************************************/

void
MenuTable::AddItem()
{
	if (EndEditing())
	{
		ItemInfo info(JXMenu::kPlainType, 0,
					  jnew JString, jnew JString,
					  JUtf8Character(' '), jnew JString, false);
		itsItemList->AppendItem(info);
		AppendRows(1);
		BeginEditing(JPoint(kTextColumn, itsItemList->GetItemCount()));
	}
}

/******************************************************************************
 DuplicateSelectedItems (private)

 ******************************************************************************/

void
MenuTable::DuplicateSelectedItems()
{
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell) && EndEditing())
	{
		itsItemList->AppendItem(itsItemList->GetItem(cell.y).Copy());
		AppendRows(1);
		BeginEditing(JPoint(kTextColumn, itsItemList->GetItemCount()));
	}
}

/******************************************************************************
 RemoveSelectedItem

 ******************************************************************************/

void
MenuTable::RemoveSelectedItem()
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
	{
		CancelEditing();

		ItemInfo info = itsItemList->GetItem(cell.y);
		info.Free();

		itsItemList->RemoveItem(cell.y);
		RemoveRow(cell.y);
	}
}

/******************************************************************************
 UpdateTypeMenu (private)

 ******************************************************************************/

void
MenuTable::UpdateTypeMenu()
{
	JPoint cell;
	const bool ok = GetTableSelection().GetSingleSelectedCell(&cell);
	assert( ok );

	const ItemInfo info = itsItemList->GetItem(cell.y);

	itsTypeMenu->CheckItem(info.type+1);
}

/******************************************************************************
 HandleTypeMenu (private)

 ******************************************************************************/

void
MenuTable::HandleTypeMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const bool ok = GetTableSelection().GetSingleSelectedCell(&cell);
	assert( ok );

	ItemInfo info = itsItemList->GetItem(cell.y);
	info.type     = (JXMenu::ItemType) (index-1);
	itsItemList->SetItem(cell.y, info);
	TableRefreshCell(cell);
}

/******************************************************************************
 HandleIconMenu (private)

 ******************************************************************************/

void
MenuTable::HandleIconMenu
	(
	const JIndex index
	)
{
	// todo
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
MenuTable::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
	{
		return;
	}

	ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
MenuTable::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	WriteGeometry(output);
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
MenuTable::ReadGeometry
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
	{
		JCoordinate w;
		input >> w;
		SetColWidth(kTypeColumn, w);
		input >> w;
		SetColWidth(kIconColumn, w);
		input >> w;
		SetColWidth(kTextColumn, w);
		input >> w;
		SetColWidth(kShortcutColumn, w);
		input >> w;
		SetColWidth(kWindowsKeyColumn, w);
		input >> w;
		SetColWidth(kIDColumn, w);
		input >> w;
		SetColWidth(kSeparatorColumn, w);
	}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
MenuTable::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion << std::endl;
	output << GetColWidth(kTypeColumn) << std::endl;
	output << GetColWidth(kIconColumn) << std::endl;
	output << GetColWidth(kTextColumn) << std::endl;
	output << GetColWidth(kShortcutColumn) << std::endl;
	output << GetColWidth(kWindowsKeyColumn) << std::endl;
	output << GetColWidth(kIDColumn) << std::endl;
	output << GetColWidth(kSeparatorColumn) << std::endl;
	output << kGeometryDataEndDelimiter << std::endl;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
MenuTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	widget->SetColumnTitles("MenuTable", kColCount);
}

/******************************************************************************
 ReadMenuItems

 ******************************************************************************/

void
MenuTable::ReadMenuItems
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (input.fail() || vers > kCurrentDataVersion)
	{
		return;
	}

	itsItemList->DeleteAll();
	RemoveAllRows();

	while (true)
	{
		bool keepGoing;
		input >> keepGoing;
		if (input.fail() || !keepGoing)
		{
			break;
		}

		ItemInfo info = ReadMenuItem(input, vers);
		itsItemList->AppendItem(info);
	}

	AppendRows(itsItemList->GetItemCount());
}

/******************************************************************************
 ReadMenuItem (static)

 ******************************************************************************/

MenuTable::ItemInfo
MenuTable::ReadMenuItem
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	assert( vers <= kCurrentDataVersion );

	int type;
	input >> type;

	JIndex iconIndex;
	input >> iconIndex;

	auto* text = jnew JString;
	input >> *text;

	auto* shortcut = jnew JString;
	input >> *shortcut;

	JUtf8Character key;
	input >> std::ws >> key;

	auto* id = jnew JString;
	input >> *id;

	bool separator;
	input >> separator;

	return ItemInfo((JXMenu::ItemType) type, iconIndex, text, shortcut, key, id, separator);
}

/******************************************************************************
 WriteMenuItems

 ******************************************************************************/

void
MenuTable::WriteMenuItems
	(
	std::ostream& output
	)
	const
{
	output << kCurrentDataVersion << std::endl;

	for (const auto& item : *itsItemList)
	{
		output << true << std::endl;
		WriteMenuItem(output, item);
		output << std::endl;
	}

	output << false;
}

/******************************************************************************
 WriteMenuItem (static)

 ******************************************************************************/

void
MenuTable::WriteMenuItem
	(
	std::ostream&	output,
	const ItemInfo&	item
	)
{
	output << item.type << std::endl;
	output << item.iconIndex << std::endl;
	output << *item.text << std::endl;
	output << *item.shortcut << std::endl;
	output << item.windowsKey << std::endl;
	output << *item.id << std::endl;
	output << item.separator << std::endl;
}

/******************************************************************************
 ItemList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
MenuTable::ItemList::DeleteAll()
{
	for (auto info : *this)
	{
		info.Free();
	}
	RemoveAll();
}

/******************************************************************************
 ItemInfo functions

 ******************************************************************************/

/******************************************************************************
 Copy

 ******************************************************************************/

MenuTable::ItemInfo
MenuTable::ItemInfo::Copy()
	const
{
	ItemInfo info = *this;
	info.text     = jnew JString(*this->text);
	info.shortcut = jnew JString(*this->shortcut);
	info.id       = jnew JString(*this->id);
	return info;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
MenuTable::ItemInfo::Free()
{
	jdelete text;
	text = nullptr;

	jdelete shortcut;
	shortcut = nullptr;

	jdelete id;
	id = nullptr;
}
